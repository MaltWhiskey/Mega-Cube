/* MIT License

Copyright (c) 2020 Ward Ramsdell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <Arduino.h>
#include "TDWS2811.h"

TDWS2811 *TDWS2811::pTD={nullptr};

TDWS2811::TDWS2811() {
  /* Get a FlexIO channel */
  pFlex = FlexIOHandler::flexIOHandler_list[FLEXMODULE];
  
  /* Pointer to the port structure in the FlexIO channel */
  p = &pFlex->port();
  
  /* Pointer to the hardware structure in the FlexIO channel */
  hw = &pFlex->hardware();
  
  /* Now configure all the things */
  configurePins();
  configurePll();
  configureFlexIO();
  configureDma();
}

void TDWS2811::_dmaIsr(void) {
  TDWS2811::pTD->dmaIsr();
}

void TDWS2811::dmaIsr(void) {
  /* Disable interrupts on TCD 1.  If there's a "disable interrupt" function in DMASetting, we should use that, but I don't see one */
  TDWS2811::dmaSetting[1].TCD->CSR &= ~DMA_TCD_CSR_INTMAJOR;

  /* Swap the buffer pointer in TCD 0 */
  if (TDWS2811::activeBuffer==0) {
    TDWS2811::activeBuffer=1;
    TDWS2811::dmaSetting[0].sourceBuffer(frameBuffer[1],24*LEDCOUNT*4);
  }
  else {
    TDWS2811::activeBuffer=0;
    TDWS2811::dmaSetting[0].sourceBuffer(frameBuffer[0],24*LEDCOUNT*4);
  }

  /* Clear the interrupt so we don't get triggered again */
  TDWS2811::dmaChannel.clearInterrupt();

  /* Spin for a few cycles.  If we don't do this, the interrupt doesn't clear and we get triggered a second time */
  for (uint8_t i=0;i<10;i++) __asm__ __volatile__ ("nop\n\t");  //Some race condition between clearInterrupt() and the return of the ISR.  If we don't delay here, the ISR will fire again.
}

void TDWS2811::configurePins(void) {
  /* Basic pin setup */
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  /* High speed and drive strength configuration */
  uint32_t pinConfig;
  pinConfig=IOMUXC_PAD_DSE(7)+IOMUXC_PAD_SPEED(3)+~IOMUXC_PAD_PKE+~IOMUXC_PAD_SRE;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_04 |= pinConfig;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_04 &= ~pinConfig;
  pinConfig=IOMUXC_PAD_DSE(6)+IOMUXC_PAD_SPEED(3)+~IOMUXC_PAD_PKE+~IOMUXC_PAD_SRE;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_05 |= pinConfig;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_05 &= ~pinConfig;
  pinConfig=IOMUXC_PAD_DSE(7)+IOMUXC_PAD_SPEED(3)+~IOMUXC_PAD_PKE+~IOMUXC_PAD_SRE;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_06 |= pinConfig;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_06 &= ~pinConfig;  
}

void TDWS2811::configureFlexIO(void) {
  /* Map the FlexIO pins */
  pFlex->mapIOPinToFlexPin(2);
  pFlex->mapIOPinToFlexPin(3);
  pFlex->mapIOPinToFlexPin(4);

  /* And set up the pin mux */
  pFlex->setIOPinToFlexMode(2);
  pFlex->setIOPinToFlexMode(3);
  pFlex->setIOPinToFlexMode(4);

  /* Enable the clock */
  hw->clock_gate_register |= hw->clock_gate_mask;

  /* Enable the FlexIO */
  p->CTRL = FLEXIO_CTRL_FLEXEN;

  /* Shifter configuration, see reference manual for a description of each register */
  /* Note that shifter 4, although unused, must be configured and tied into the chain.  If it's not, there's a data glitch that trips us up.
  /* See the Github README for more details. */
  p->SHIFTCTL[0] = 0x00830602;
  p->SHIFTCTL[1] = 0x00800002;
  p->SHIFTCTL[2] = 0x00800002;
  p->SHIFTCTL[3] = 0x00800002;
  p->SHIFTCTL[4] = 0x00800002;

  p->SHIFTCFG[0] = 0x00000100;
  p->SHIFTCFG[1] = 0x00000100;
  p->SHIFTCFG[2] = 0x00000100;
  p->SHIFTCFG[3] = 0x00000100;
  p->SHIFTCFG[4] = 0x00000100;

  /* Timer configuration, see reference manual for a description of each register */
  p->TIMCFG[0] =   0x00000200;
  p->TIMCFG[1] =   0x00000100;

  p->TIMCTL[0] =   0x01C30401;
  p->TIMCTL[1] =   0x00030503;

  p->TIMCMP[0] =   0x0000BF00;
  p->TIMCMP[1] =   0x0000001F;

  /* Finally, set up the values to be loaded into the shift registers at the beginning of each bit */
  p->SHIFTBUF[0] = 0xFFFFFFFF;
  p->SHIFTBUFBIS[1] = 0xAAAAAAAA;  //Identifiable pattern should DMA fail to write SHIFTBUFBIS[1]
  p->SHIFTBUF[2] = 0x00000000;
  p->SHIFTBUF[3] = 0x00000000;
  p->SHIFTBUF[3] = 0x00000000;

}

void TDWS2811::configurePll (void) {
  /* Set up PLL5 (also known as "PLL_VIDEO" and "PLL_528"), connect to FlexIO1 */
  uint32_t pllVideo;

  /* Disable the Video PLL output before initial Video PLL configuration */
  CCM_ANALOG_PLL_VIDEO &= ~CCM_ANALOG_PLL_VIDEO_ENABLE_MASK;

  /* Bypass PLL first */
  CCM_ANALOG_PLL_VIDEO = (CCM_ANALOG_PLL_VIDEO & (~CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC_MASK)) |
                          CCM_ANALOG_PLL_VIDEO_BYPASS_MASK | CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC(0);

  /* Set numerator and denominator */
  CCM_ANALOG_PLL_VIDEO_NUM = CCM_ANALOG_PLL_VIDEO_NUM_A(0);
  CCM_ANALOG_PLL_VIDEO_DENOM = CCM_ANALOG_PLL_VIDEO_DENOM_B(12);

  /* Set DIV */
  pllVideo = (CCM_ANALOG_PLL_VIDEO & (~(CCM_ANALOG_PLL_VIDEO_DIV_SELECT_MASK | CCM_ANALOG_PLL_VIDEO_POWERDOWN_MASK))) |
             CCM_ANALOG_PLL_VIDEO_ENABLE_MASK |CCM_ANALOG_PLL_VIDEO_DIV_SELECT(32);

  /* Set the post divider.  To reduce operation by a factor of 4 (for debugging) change from a divide ratio of 2 to 4 */
  pllVideo |= CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT(2);
//  pllVideo |= CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT(4);

  /* Write the PLL divider and post divider to the configuration register */
  CCM_ANALOG_PLL_VIDEO = pllVideo;

  /* Don't remember, TODO: remember */
  CCM_ANALOG_MISC2 = (CCM_ANALOG_MISC2 & (~CCM_ANALOG_MISC2_VIDEO_DIV_MASK)) | CCM_ANALOG_MISC2_VIDEO_DIV(2);

  /* Wait for the PLL to lock.  If it doesn't lock, well, wait some more. */
  while ((CCM_ANALOG_PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_LOCK_MASK) == 0)
  {
  }
  
  /* Disable bypass for Video PLL once it's locked. */
  CCM_ANALOG_PLL_VIDEO &= ~(uint32_t(CCM_ANALOG_PLL_VIDEO_BYPASS_MASK));

  /* Disable FlexIO clock gate */
  hw->clock_gate_register &= ~(uint32_t(hw->clock_gate_mask));

  /* Set FLEXIO1_CLK_PRED. Set PODF=0 for full speed operation, PDOF=7 for 1/8 speed operation for debugging */
  CCM_CDCDR = (CCM_CDCDR & ~(CCM_CDCDR_FLEXIO1_CLK_SEL(3) | CCM_CDCDR_FLEXIO1_CLK_PRED(7) | CCM_CDCDR_FLEXIO1_CLK_PODF(7))) 
      | CCM_CDCDR_FLEXIO1_CLK_SEL(2) | CCM_CDCDR_FLEXIO1_CLK_PRED(4) | CCM_CDCDR_FLEXIO1_CLK_PODF(0);
//      | CCM_CDCDR_FLEXIO1_CLK_SEL(2) | CCM_CDCDR_FLEXIO1_CLK_PRED(4) | CCM_CDCDR_FLEXIO1_CLK_PODF(7);
}

void TDWS2811::configureDma() {
  /* Enable DMA trigger on Shifter 1 */
  p->SHIFTSDEN |= 0X00000002;

  /* TCD 0 is responsible for the bulk of the data transfer.  It shuffles data from the frame buffer to Shifter 1 */
  dmaSetting[0].sourceBuffer(frameBuffer[0],24*LEDCOUNT*4);
  dmaSetting[0].destination(p->SHIFTBUFBIS[1]);
  dmaSetting[0].replaceSettingsOnCompletion(dmaSetting[1]);

  /* TCD 1 is responsible for setting Shifter 0 (Phase 1) to zero.  I tried "source(zeros[0])", but it doesn't work.  Maybe that's a byte-wide operation?
  Note that the DMA channel is triggered by FlexIO shifter 1.  The only way to acknowledge the trigger is to write to one of Shifter 1's data registers.
  TCD 1 only touches Shifter 0's register, so the DMA channel is immediately triggered again upon TCD 1's completion, moving us directly into TCD 2. */
  dmaSetting[1].sourceBuffer(zeros,4);
  dmaSetting[1].destination(p->SHIFTBUF[0]);
  dmaSetting[1].replaceSettingsOnCompletion(dmaSetting[2]);

  /* TCD 2 is responsible for the blanking delay.  That delay is, in turn, dictated by the size of the "zeros" array.
  As this TCD executes once per (1.25us) bit period, an array of 40 zeros gets us a 50us delay.*/
  dmaSetting[2].sourceBuffer(zeros,sizeof(zeros));
  dmaSetting[2].destination(p->SHIFTBUF[1]);
  dmaSetting[2].replaceSettingsOnCompletion(dmaSetting[3]);

  /* TCD 3 is responsible for setting Shifter 0 (Phase 1) to ones.  I tried "source(&ones)", but it doesn't work.  Maybe that's a byte-wide operation?
  Note that the DMA channel is triggered by FlexIO shifter 1.  The only way to acknowledge the trigger is to write to one of Shifter 1's data registers.
  TCD 3 only touches Shifter 0's register, so the DMA channel is immediately triggered again upon TCD 3's completion, moving us directly into TCD 0.*/
  dmaSetting[3].sourceBuffer(&ones,4);
  dmaSetting[3].destination(p->SHIFTBUF[0]);
  dmaSetting[3].replaceSettingsOnCompletion(dmaSetting[0]);

  /* Now set up the DMA channel and initialize it with TCD 0 */
  dmaChannel.disable();
  dmaChannel=dmaSetting[0];
  dmaChannel.triggerAtHardwareEvent(hw->shifters_dma_channel[1]);
  dmaChannel.attachInterrupt(&_dmaIsr);
  /* Needed for the ISR, for reasons relating to using C++ in an embedded environment, which is most foolish */
  pTD=this;
  dmaChannel.enable();
}

void TDWS2811::flipBuffers(void){
  /* Swap active and inactive frame buffers.  This function simply sets the ISR on completion flag of TCD 1 and that ISR handles the rest.
  This is to synchronize the buffer swap with the frame blanking period in order to prevent tearing. */
  dmaSetting[1].TCD->CSR |= DMA_TCD_CSR_INTMAJOR;
}


int TDWS2811::setLed(uint8_t channel, uint8_t led, color_t color, bufferType_t writeBuffer) {
  /* Set an LED's color and intensity */
  if (led>LEDCOUNT)
  {
    return 0;
  }
  if (channel>31)
  {
    return 0;
  }

  uint16_t base=0;
  uint32_t ledVal=0;
  uint8_t bitCount=0;
  uint8_t buf=activeBuffer;
  if (writeBuffer==INACTIVE) buf=1-activeBuffer;
  
  if (channelType[channel]==RGB)
  {
    bitCount=24;
    ledVal=(color.red<<24)+(color.green<<16)+(color.blue<<8);
  }
  
  else if (channelType[channel]==GRB)
  {
    bitCount=24;
    ledVal=(color.green<<24)+(color.red<<16)+(color.blue<<8);
  }

  else if (channelType[channel]==GRBW)
  {
    bitCount=32;
    ledVal=(color.green<<24)+(color.red<<16)+(color.blue<<8)+color.white;
  }

  else
  {
    return 0;
  }

  uint8_t i;
  uint32_t mask = 1<<channel;

  base=bitCount*led;
  for (i=0;i<bitCount;i++)
  {
    if (ledVal&0x80000000) 
    {
      frameBuffer[buf][base+i] |= mask;
    }
    else
    {
      frameBuffer[buf][base+i] &= ~mask;
    }
    ledVal=ledVal<<1;
  }
  
  return 1;
}

color_t TDWS2811::getLed(uint8_t channel, uint16_t led) {
  /* Returns the color and intensity value of an LED */
  uint32_t mask=1<<channel;
  uint8_t i, bitCount=24;
  if (channelType[channel]==GRBW) bitCount=32;
  uint16_t base=led*bitCount;
  uint32_t out=0;
  color_t color;
  for (i=0;i<bitCount;i++)
  {
    out<<=1;
    if (frameBuffer[activeBuffer][base+i] & mask) out += 1;
  }
  
  switch (channelType[channel])
  {
    case RGB:
      color.blue = out & 0xFF;
      out>>=8;
      color.green = out & 0xFF;
      out>>=8;
      color.red = out & 0xFF;
      break;

    case GRB:
      color.blue = out & 0xFF;
      out>>=8;
      color.red = out & 0xFF;
      out>>=8;
      color.green = out & 0xFF;
      break;

    case GRBW:
      color.white = out & 0xFF;
      out>>=8;
      color.blue = out & 0xFF;
      out>>=8;
      color.red = out & 0xFF;
      out>>=8;
      color.green = out & 0xFF;
      break;

    default:
      return {0,0,0,0};
  }
  return color;
}

void TDWS2811::setChannelType(uint8_t channel, channelType_t chanType)
{
  /* Allows the user to change each channel to RGB, GRB, or GRBW formatting */
  channelType[channel]=chanType;
}

uint32_t *TDWS2811::getActiveBuffer(void)
{
  /* Returns a pointer to the active frame buffer.  Useful for developing more sophisticated buffer writing algorithms */
  return frameBuffer[activeBuffer];
}

uint32_t *TDWS2811::getInactiveBuffer(void)
{
  /* Returns a pointer to the inactive frame buffer.  Useful for developing more sophisticated buffer writing algorithms */
  return frameBuffer[1-activeBuffer];
}

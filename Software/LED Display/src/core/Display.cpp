#include "display.h"

#include <Arduino.h>

#include "timer.h"
/*------------------------------------------------------------------------------
 * DISPLAY CLASS
 *----------------------------------------------------------------------------*/
void Display::begin() {
  setupPLL();
  setupFIO();
  setupDMA();
}
// Static interrupt handler, delegate to the only instance
void Display::interruptAtCompletion(void) {
  Display::instance().displayReady();
}
// Triggered after all led data is sent but before reset/latch
void Display::displayReady(void) {
  // First clear the interrupt flag to avoid retriggering
  dmaChannel[0].clearInterrupt();
  // Swap dma buffer if a new one is available
  if (!m_displayAvailable) {
    // The prep buffer becomes the dma buffer and visa versa
    m_dmaBuffer = 1 - m_dmaBuffer;
    dmaSetting[0].sourceBuffer(dmaBufferData[m_dmaBuffer],
                               sizeof(dmaBufferData[0]));
    dmaSetting[4].sourceBuffer(dmaBufferData[m_dmaBuffer],
                               sizeof(dmaBufferData[0]));
  }
  // The display is available to accept cube data
  m_displayAvailable = true;
}

// Notifies the display that a new frame is ready for displaying. Transfer the
// cube data to the prep buffer and enable the interrupt to swap the buffers.
void Display::update() {
  // Create the dma buffer from the prep buffer.
  for (uint8_t z = 0; z < depth; z++) {
    for (uint8_t y = 0; y < height; y++) {
      for (uint8_t x = 0; x < width; x++) {
        Color c = color(x, y, z);
        c = c.scale(64);
        uint8_t chn = ((x >> 1) & 0x0E) + ((z << 1) & 0xF8) + ((z >> 1) & 0x01);
        //        uint8_t led = ((x << 4) & 0x30) + ((y ^ (-(x & 1))) & 0x0F) +
        //                      ((y ^ (-(z & 1))) & 0x8F);
        uint8_t led = (x << 4) & 0x30;
        if (x & 1) {
          led += (15 - y);
        } else
          led += y;
        if (z & 1) {
          led = 127 - led;
        }
        uint32_t mask = 1 << chn;
        uint32_t value = (c.red << 24) + (c.green << 16) + (c.blue << 8);
        uint16_t offset = BITCOUNT * led;
        for (uint8_t i = 0; i < BITCOUNT; i++) {
          if (value & 0x80000000)
            dmaBufferData[1 - m_dmaBuffer][offset++] |= mask;
          else
            dmaBufferData[1 - m_dmaBuffer][offset++] &= ~mask;
          value <<= 1;
        }
      }
    }
  }
  // Writing to the cube data is not allowed to prevent frame tearing
  m_displayAvailable = false;
}

// Check if the display is available to accept new cube data
bool Display::available() { return m_displayAvailable; }

// Clear the cube so a new frame can be freshly created.
void Display::clear() { memset(cube, 0, sizeof(cube)); }

const uint8_t Display::gamma8[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
    2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,
    4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,
    8,   9,   9,   9,   10,  10,  10,  11,  11,  11,  12,  12,  13,  13,  13,
    14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,
    21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,
    31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,  42,
    43,  44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  54,  55,  56,  57,
    58,  59,  60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  72,  73,  74,
    75,  77,  78,  79,  81,  82,  83,  85,  86,  87,  89,  90,  92,  93,  95,
    96,  98,  99,  101, 102, 104, 105, 107, 109, 110, 112, 114, 115, 117, 119,
    120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142, 144, 146,
    148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175, 177,
    180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252,
    255};

/****************************************************************************
 * Set up PLL5 (also known as "VIDEO PLL")
 * This configures the Clock Controller Module (CCM)
 *
 * The internal pll clock is set to 24MHz, the frequency generated is
 * 24 * (DIV_SELECT + NUM/DENOM) -> 24 * (42 + 2/3) = 1024 MHz
 * 24 * (DIV_SELECT + NUM/DENOM) -> 24 * (29 + 17/27) = 711.1111111 MHz
 ***************************************************************************/
void Display::setupPLL() {
  // Before disabeling the PLL set the bypass source to the internal 24MHz
  // reference clock. See 14.6.1.6 page 1039.
  CCM_ANALOG_PLL_VIDEO_CLR = CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC(3) |
                             // Clear power down bit to power up the PLL
                             CCM_ANALOG_PLL_VIDEO_POWERDOWN;
  // Bypass the PLL also see 13.3.2.2.1 page 987
  CCM_ANALOG_PLL_VIDEO_SET = CCM_ANALOG_PLL_VIDEO_BYPASS;
  // Disable the Video PLL output before configurating
  CCM_ANALOG_PLL_VIDEO_CLR = CCM_ANALOG_PLL_VIDEO_ENABLE;
  // Clear dividers before setting the values
  CCM_ANALOG_PLL_VIDEO_CLR = CCM_ANALOG_PLL_VIDEO_DIV_SELECT(0x7f) |
                             CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT(3);
#if defined PL9823
  // NUM = 30 bits signed numer, abs(NUM) must be less than DENOM
  CCM_ANALOG_PLL_VIDEO_NUM = 17;
  // DENOM = 30 bits unsigned numer. NUM/DENOM -> fracional loop diver
  CCM_ANALOG_PLL_VIDEO_DENOM = 27;
#else
  // NUM = 30 bits signed numer, abs(NUM) must be less than DENOM
  CCM_ANALOG_PLL_VIDEO_NUM = 2;
  // DENOM = 30 bits unsigned numer. NUM/DENOM -> fracional loop diver
  CCM_ANALOG_PLL_VIDEO_DENOM = 3;
#endif
  // Clear dividers before setting the values
  CCM_ANALOG_MISC2_CLR = CCM_ANALOG_MISC2_VIDEO_DIV(3);
  // Post-divider for video values (0=/1, 1=/2, 2=/1, 3=/4) (2 is also /1)
  CCM_ANALOG_MISC2_SET = CCM_ANALOG_MISC2_VIDEO_DIV(0);

  CCM_ANALOG_PLL_VIDEO_SET =
  // DIV_SELECT set the loop divider (27-54)
#if defined PL9823
      CCM_ANALOG_PLL_VIDEO_DIV_SELECT(29) |
#else
      CCM_ANALOG_PLL_VIDEO_DIV_SELECT(42) |
#endif
      // Divider after PLL (0=/4, 1=/2, 2=/1, 3=reserved)
      CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT(2) |
      // Enable PLL output (still bypassed)
      CCM_ANALOG_PLL_VIDEO_ENABLE;

  // Wait for the PLL to lock. Prevents random initial edges 13.3.2.2.1
  while ((CCM_ANALOG_PLL_VIDEO & CCM_ANALOG_PLL_VIDEO_LOCK) == 0) {
  }
  // Disable bypass for Video PLL once it's locked.
  CCM_ANALOG_PLL_VIDEO_CLR = CCM_ANALOG_PLL_VIDEO_BYPASS;
}

/*******************************************************************************
 * Configure flexio
 *
 * Pad settings:
 *   HYS Hysteresis enable = 0 (0=disabled, 1=enabled)
 *   PUS pull up/down config select = 0
 *     (0=100K pull down, 1=47K pull up, 2=100K pull up, 3=22K pull up)
 *   PUE Keeper select = 0 (0=keeper, 1=pull)
 *   PKE Pull keeper enable = 0 (0=disabled, 1=enabled)
 *   ODE Open drain enable = 0 (0=disabled, 1=enabled)
 *   SPEED speed = 1
 *     (0=50MHz, 1=100MHz, 2=150MHz, 3=200MHz)
 *   DSE drive strength = 7 (should be impedance matched)
 *     (0 = off, 1=150/1 Ohm, 2=150/2 Ohm, 3=150/3 Ohm, ... 7=150/7 Ohm)
 *   SRE slew rate = 0 (0=slow, 1=fast)
 ******************************************************************************/
void Display::setupFIO() {
  *portModeRegister(DIN) |= digitalPinToBitMask(DIN);
  *portControlRegister(DIN) = IOMUXC_PAD_DSE(7) | IOMUXC_PAD_SPEED(1);
  // SION + ALT4 (FLEXIO2_FLEXIO16) (IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00)
  *portConfigRegister(DIN) = 0x14;

  *portModeRegister(WCK) |= digitalPinToBitMask(WCK);
  *portControlRegister(WCK) = IOMUXC_PAD_DSE(7) | IOMUXC_PAD_SPEED(1);
  // SION + ALT4 (FLEXIO2_FLEXIO11) (IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_11)
  *portConfigRegister(WCK) = 0x14;

  *portModeRegister(BCK) |= digitalPinToBitMask(BCK);
  *portControlRegister(BCK) = IOMUXC_PAD_DSE(6) | IOMUXC_PAD_SPEED(1);
  // SION + ALT4 (FLEXIO2_FLEXIO00) (IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00)
  *portConfigRegister(BCK) = 0x14;

  // Disable flexio2 clock gate see 14.7.24 page 1087
  CCM_CCGR3 &= ~CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
  // Serial Clock Multiplexer Register 2
  CCM_CSCMR2 &= ~CCM_CSCMR2_FLEXIO2_CLK_SEL(3);
  // Derive clock from PLL5 clock 14.7.8 page 1059
  CCM_CSCMR2 |= CCM_CSCMR2_FLEXIO2_CLK_SEL(2);
  // Clock divider register pre divider
  CCM_CS1CDR &= ~CCM_CS1CDR_FLEXIO2_CLK_PRED(7);
  // Divide by (4 + 1 = 5)
  CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PRED(4);
  // Clock divider register post divider
  CCM_CS1CDR &= ~CCM_CS1CDR_FLEXIO2_CLK_PODF(7);
  // Divide by (0 + 1 = 1)
  CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PODF(0);
  // Enable flexio2 clock gate see 14.7.24 page 1087
  CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);

  // Shifter control 50.5.1.14 page 2925
  // SHIFTER0 is configured to output on DIN = pin 8
  IMXRT_FLEXIO2_S.SHIFTCTL[0] =
      // Timer select -> TIMER0 controls logic and shift clock
      FLEXIO_SHIFTCTL_TIMSEL(0) |
      // Timer polarity -> shift on negative edge of shift clock
      FLEXIO_SHIFTCTL_TIMPOL |
      // Shifter pin configuration -> shifter pin output
      FLEXIO_SHIFTCTL_PINCFG(3) |
      // Shifter pin select FLEXIO16 pin (DIN pin)
      FLEXIO_SHIFTCTL_PINSEL(16) |
      // Shifter pin polarity -> pin is active high
      (FLEXIO_SHIFTCTL_PINPOL & 0) |
      // Shifter mode -> Transmit mode. Load SHIFTBUF contents into
      // the shifter on expiration of the Timer
      FLEXIO_SHIFTCTL_SMOD(2);
  // SHIFTERS 1-3 do not output to a pin
  IMXRT_FLEXIO2_S.SHIFTCTL[1] =
      FLEXIO_SHIFTCTL_TIMPOL | FLEXIO_SHIFTCTL_SMOD(2);
  IMXRT_FLEXIO2_S.SHIFTCTL[2] =
      FLEXIO_SHIFTCTL_TIMPOL | FLEXIO_SHIFTCTL_SMOD(2);
  IMXRT_FLEXIO2_S.SHIFTCTL[3] =
      FLEXIO_SHIFTCTL_TIMPOL | FLEXIO_SHIFTCTL_SMOD(2);

  // Shifter configuration 50.5.1.15 page 2927
  // SHIFTER0 shifts 1 bit on each clock and has SHIFTER(0+1) as source
  IMXRT_FLEXIO2_S.SHIFTCFG[0] =
      // 1-bit shift on each shift clock
      FLEXIO_SHIFTCFG_PWIDTH(0) |
      // Input source for shifter is output of Shifter N+1
      FLEXIO_SHIFTCFG_INSRC;
  // Same for shifters 1 - 3
  IMXRT_FLEXIO2_S.SHIFTCFG[1] = FLEXIO_SHIFTCFG_INSRC;
  IMXRT_FLEXIO2_S.SHIFTCFG[2] = FLEXIO_SHIFTCFG_INSRC;
  // Illegal input source for shifter 3 ??????
  IMXRT_FLEXIO2_S.SHIFTCFG[3] = FLEXIO_SHIFTCFG_INSRC;

  // Timer configuration 50.5.1.21.4 page 2935
  IMXRT_FLEXIO2_S.TIMCFG[0] =
      // Timer output is logic 1 when enabled, not affected by reset
      FLEXIO_TIMCFG_TIMOUT(0) |
      // Decrement counter on flexio clock, shift clock = timer output
      FLEXIO_TIMCFG_TIMDEC(0) |
      // Timer never resets
      FLEXIO_TIMCFG_TIMRST(0) |
      // Timer never disabled
      FLEXIO_TIMCFG_TIMDIS(0) |
      // Timer enabled on trigger high (shifter 0 status flag)
      FLEXIO_TIMCFG_TIMENA(2) |
      // Stop bit disabled
      FLEXIO_TIMCFG_TSTOP(0) |
      // Start bit disabled
      (FLEXIO_TIMCFG_TSTART & 0);
  IMXRT_FLEXIO2_S.TIMCFG[1] =
      // Timer enabled on timer N-1 enable
      FLEXIO_TIMCFG_TIMENA(1);

  // Timer control 50.5.1.20 page 2932
  // Triggered after loading SHIFTER0 from SHIFTBUF0 50.5.1.16.3 page 2929
  // TIMER0 is configured to output on BCK = pin 10
  IMXRT_FLEXIO2_S.TIMCTL[0] =
      // Trigger select -> triggers on SHIFTER[N=0] status flag (4*N+1)
      FLEXIO_TIMCTL_TRGSEL(1) |
      // Trigger polarity -> trigger active low
      FLEXIO_TIMCTL_TRGPOL |
      // Trigger source -> internal trigger selected
      FLEXIO_TIMCTL_TRGSRC |
      // Timer pin configuration -> timer pin output
      FLEXIO_TIMCTL_PINCFG(3) |
      // Timer pin select -> FLEXIO00 (BCK on pin 10)
      FLEXIO_TIMCTL_PINSEL(0) |
      // Timer pin polarity -> active high
      (FLEXIO_TIMCTL_PINPOL & 0) |
      // Timer mode -> dual 8 bit counters baud mode
      FLEXIO_TIMCTL_TIMOD(1);
  // No trigger is used, timer is enabled on TIMER0
  // TIMER1 is configured to output on BCK = pin 10
  IMXRT_FLEXIO2_S.TIMCTL[1] =
      // Timer pin configuration -> timer pin output
      FLEXIO_TIMCTL_PINCFG(3) |
      // Timer pin select -> FLEXIO11 (WCK on pin 9)
      FLEXIO_TIMCTL_PINSEL(11) |
      // Timer pin polarity -> active high
      (FLEXIO_TIMCTL_PINPOL & 0) |
      // Timer mode -> single 16-bit counter mode
      FLEXIO_TIMCTL_TIMOD(3);

  // Timer compare 50.5.1.22 page 2937
  // The upper 8 bits configure the number of bits = (cmp[15:8] + 1) / 2
  // Upper 8 bits -> 4 x 32 bits -> 128 * 2 -> 256 - 1 = 0xFF voor 128 bits
  // The lower 8 bits configure baud rate divider = (cmp[ 7:0] + 1) * 2
  // Lower 8 bits -> (0 + 1) * 2 -> divide frequency by 2
  IMXRT_FLEXIO2_S.TIMCMP[0] = 0x0000FF00;
  // Timer compare 50.3.3.3 page 2891
  // Configure baud rate of the shift clock (cmp[15:0] + 1) * 2
  // -> (31 + 1) * 2 -> divide frequency by 64
  // When the counter equals zero and decrements the timer output toggles
  // Pulses generated -> 32 pulses of TIMER0 and 1 pulse of TIMER1
  IMXRT_FLEXIO2_S.TIMCMP[1] = 0x0000001F;

  // Shiftbuffers 1 & 2 get filled by DMA later. Start with all zero's
  // to reset/latch the led's. See 50.5.1.6.3 page 2918 for DMA triggering.
  IMXRT_FLEXIO2_S.SHIFTBUFBIS[0] = 0x00000000;
  IMXRT_FLEXIO2_S.SHIFTBUFBIS[1] = 0x00000000;
  IMXRT_FLEXIO2_S.SHIFTBUFBIS[2] = 0x00000000;
  IMXRT_FLEXIO2_S.SHIFTBUFBIS[3] = 0x00000000;
  // Enable DMA trigger when SHIFTBUF[1 or 2] is loaded onto the shifter
  IMXRT_FLEXIO2_S.SHIFTSDEN |= 0x06;
  // Enable flexio, SHIFTBUF[0] has been written, so TIMER0 will start
  // and other buffers are also ready so no errors in shifting data
  IMXRT_FLEXIO2_S.CTRL = FLEXIO_CTRL_FLEXEN;
}

void Display::setupDMA() {
  // TCD 0 transfers the active dma buffer data to SHIFTBUF[1].
  dmaSetting[0].sourceBuffer(dmaBufferData[m_dmaBuffer],
                             sizeof(dmaBufferData[0]));
  dmaSetting[0].destination(IMXRT_FLEXIO2_S.SHIFTBUFBIS[1]);
  dmaSetting[0].replaceSettingsOnCompletion(dmaSetting[1]);
  // Synchronize buffer swapping with dma transfer to avoid display artifacts
  dmaSetting[0].interruptAtCompletion();
  // TCD 1 sets SHIFTBUF[0] Low. The DMA channel is triggered by FlexIO
  // shifter 1. A write to SHIFTBUF[0] doesn't clear this trigger. The DMA
  // channel is triggered again upon completion of TCD1
  dmaSetting[1].sourceBuffer(dmaBufferLow, 4);
  dmaSetting[1].destination(IMXRT_FLEXIO2_S.SHIFTBUFBIS[0]);
  dmaSetting[1].replaceSettingsOnCompletion(dmaSetting[2]);
  // TCD 2 sets SHIFTBUF[1] to 0. This is to reset the leds.
  // We need >50us of zeros so adjust buffer size accordingly.
  dmaSetting[2].sourceBuffer(dmaBufferLow, sizeof(dmaBufferLow));
  dmaSetting[2].destination(IMXRT_FLEXIO2_S.SHIFTBUFBIS[1]);
  dmaSetting[2].replaceSettingsOnCompletion(dmaSetting[3]);
  // TCD 3 sets SHIFTBUF[0] High. Triggers again.
  dmaSetting[3].sourceBuffer(dmaBufferHigh, 4);
  dmaSetting[3].destination(IMXRT_FLEXIO2_S.SHIFTBUFBIS[0]);
  dmaSetting[3].replaceSettingsOnCompletion(dmaSetting[0]);

  // TCD 4 = TCD 0 for SHIFBUF[2]
  dmaSetting[4].sourceBuffer(dmaBufferData[m_dmaBuffer],
                             sizeof(dmaBufferData[0]));
  dmaSetting[4].destination(IMXRT_FLEXIO2_S.SHIFTBUFBIS[2]);
  dmaSetting[4].replaceSettingsOnCompletion(dmaSetting[5]);
  // TCD 5 = TCD 0 for SHIFBUF[2]
  dmaSetting[5].sourceBuffer(dmaBufferLow, sizeof(dmaBufferLow));
  dmaSetting[5].destination(IMXRT_FLEXIO2_S.SHIFTBUFBIS[2]);
  dmaSetting[5].replaceSettingsOnCompletion(dmaSetting[4]);

  // Initialize both DMA channels
  dmaChannel[0].disable();
  dmaChannel[1].disable();
  // Start with the reset/latch signal, so we can begin "fresh"
  dmaChannel[0] = dmaSetting[2];
  dmaChannel[1] = dmaSetting[5];
  dmaChannel[0].triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST1);
  // Attach interrupt to dma channel 0. The interrupt is enabled in update
  // and disabled again in the ISR.
  dmaChannel[0].attachInterrupt(&interruptAtCompletion);
  // Bit pattern for PL9823 leds: High, Data, Data, Low
  // Bit pattern for WS2812 leds: High, Data, Low, Low
#if defined PL9823
  // Quick Fix: Don't trigger DMA for WS2812, this keeps 2nd data pulse low
  dmaChannel[1].triggerAtHardwareEvent(DMAMUX_SOURCE_FLEXIO2_REQUEST3);
#endif
  dmaChannel[0].enable();
  dmaChannel[1].enable();
}
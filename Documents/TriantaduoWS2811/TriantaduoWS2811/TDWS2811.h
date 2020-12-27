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

#ifndef TDWS2811_h
#define TDWS2811_h
#include <Arduino.h>
#include <DMAChannel.h>
#include <FlexIO_t4.h>

#define FLEXMODULE 0

#define HASWHITE

#ifndef LEDCOUNT
#define LEDCOUNT 1000
#endif

#ifndef DEFAULT_CHANNEL_TYPE
#define DEFAULT_CHANNEL_TYPE GRB
#endif

#define CCM_ANALOG_PLL_VIDEO_ENABLE_SHIFT        (13U)
#define CCM_ANALOG_PLL_VIDEO_BYPASS_MASK (0x10000U)
#define CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC_MASK (0xC000U)
#define CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC_SHIFT (14U)
#define CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC(x)   (((uint32_t)(((uint32_t)(x)) << CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC_SHIFT)) & CCM_ANALOG_PLL_VIDEO_BYPASS_CLK_SRC_MASK)
#define CCM_ANALOG_PLL_VIDEO_NUM_A_MASK          (0x3FFFFFFFU)
#define CCM_ANALOG_PLL_VIDEO_NUM_A_SHIFT         (0U)
#define CCM_ANALOG_PLL_VIDEO_NUM_A(x)            (((uint32_t)(((uint32_t)(x)) << CCM_ANALOG_PLL_VIDEO_NUM_A_SHIFT)) & CCM_ANALOG_PLL_VIDEO_NUM_A_MASK)
#define CCM_ANALOG_PLL_VIDEO_DENOM_B_MASK        (0x3FFFFFFFU)
#define CCM_ANALOG_PLL_VIDEO_DENOM_B_SHIFT       (0U)
#define CCM_ANALOG_PLL_VIDEO_DENOM_B(x)          (((uint32_t)(((uint32_t)(x)) << CCM_ANALOG_PLL_VIDEO_DENOM_B_SHIFT)) & CCM_ANALOG_PLL_VIDEO_DENOM_B_MASK)
#define CCM_ANALOG_PLL_VIDEO_DIV_SELECT_MASK     (0x7FU)
#define CCM_ANALOG_PLL_VIDEO_POWERDOWN_MASK      (0x1000U)
#define CCM_ANALOG_PLL_VIDEO_ENABLE_MASK         (0x2000U)
#define CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT_MASK (0x180000U)
#define CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT_SHIFT (19U)
#define CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT(x)  (((uint32_t)(((uint32_t)(x)) << CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT_SHIFT)) & CCM_ANALOG_PLL_VIDEO_POST_DIV_SELECT_MASK)
#define CCM_ANALOG_PLL_VIDEO_DIV_SELECT_MASK     (0x7FU)
#define CCM_ANALOG_PLL_VIDEO_DIV_SELECT_SHIFT    (0U)
#define CCM_ANALOG_PLL_VIDEO_DIV_SELECT(x)       (((uint32_t)(((uint32_t)(x)) << CCM_ANALOG_PLL_VIDEO_DIV_SELECT_SHIFT)) & CCM_ANALOG_PLL_VIDEO_DIV_SELECT_MASK)
#define CCM_ANALOG_MISC2_VIDEO_DIV_MASK          (0xC0000000U)
#define CCM_ANALOG_MISC2_VIDEO_DIV_SHIFT         (30U)
#define CCM_ANALOG_MISC2_VIDEO_DIV(x)            (((uint32_t)(((uint32_t)(x)) << CCM_ANALOG_MISC2_VIDEO_DIV_SHIFT)) & CCM_ANALOG_MISC2_VIDEO_DIV_MASK)
#define CCM_ANALOG_PLL_VIDEO_LOCK_MASK           (0x80000000U)


enum channelType_t {RGB,GRB,GRBW};

enum bufferType_t {ACTIVE,INACTIVE};

typedef struct {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t white = 0;
} color_t;

class TDWS2811
{
  public:
    TDWS2811();
    void flipBuffers(void);
    uint32_t *getActiveBuffer(void);
    uint32_t *getInactiveBuffer(void);
    int setLed(uint8_t, uint8_t, color_t, bufferType_t writeBuffer=ACTIVE);
    color_t getLed(uint8_t, uint16_t);
    void setChannelType(uint8_t, channelType_t);
    
#ifdef HASWHITE    
    volatile uint32_t frameBuffer[2][32*LEDCOUNT]={{0},{0}};  /* If the collection of channels has white, size the frame buffer for 32 bits per pixel */
#else
    volatile uint32_t frameBuffer[2][24*LEDCOUNT]={{0},{0}};  /* If no white, size the frame buffer for 24 bits per pixel */
#endif

  private:
    static void _shifterIsr(void);
    static void _dmaIsr(void);
    void shifterIsr(void);
    void dmaIsr(void);
    void configurePins(void);
    void configureFlexIO(void);
    void configurePll (void);
    void configureInterrupts(void);
    void configureDma(void);
    void dumpDMA_TCD(DMAChannel *);

    FlexIOHandler *pFlex;
    DMAChannel dmaChannel;
    DMASetting dmaSetting[4];
    volatile uint8_t activeBuffer=0;
    volatile const uint32_t zeros[40]={0};
    volatile const uint32_t ones=0xFFFFFFFF;
    static TDWS2811 *pTD;
    IMXRT_FLEXIO_t *p = &pFlex->port();
    const FlexIOHandler::FLEXIO_Hardware_t *hw = &pFlex->hardware();
    uint32_t ledCnt;
    channelType_t channelType[32]={DEFAULT_CHANNEL_TYPE};
};
#endif

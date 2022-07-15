/******************************************************************************
*  ILI9341_T4 library for driving an ILI9341 screen via SPI with a Teensy 4/4.1
*  Implements vsync and differential updates from a memory framebuffer.
*
*  Copyright (c) 2020 Arvind Singh.  All right reserved.
*
* This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************/



/**
*CREDITS. Parts of this code is based on:
*
* (1) KurtE's highly optimized library for ILI9341: https://github.com/KurtE/ILI9341_t3n
*     -> for SPI / DMA and all the fancy low level hardware stuff... beautiful !
*
* (2) PJRC's XPT2048 library https://github.com/PaulStoffregen/XPT2046_Touchscreen
*     -> for all the touchscreen related methods.
**/

#ifndef _ILI9341_T4_ILI9341Driver_H_
#define _ILI9341_T4_ILI9341Driver_H_

// only c++, no plain c
#ifdef __cplusplus

#include "StatsVar.h"
#include "DiffBuff.h"

#include <Arduino.h>
#include <DMAChannel.h>
#include <SPI.h>
#include <stdint.h>

// This libray uses specify hardware features of Teensy 4/4.1 and will not work with another MCU...
#if (!defined(__IMXRT1062__))  ||  (!defined(CORE_TEENSY))
#error "This library only supports Teensy 4/4.1"
#endif



namespace ILI9341_T4
{



    /** Configuration */

#define ILI9341_T4_DEFAULT_SPICLOCK 30000000         // default SPI write speed, some display can work up to 80Mhz...
#define ILI9341_T4_DEFAULT_SPICLOCK_READ 4000000     // default SPI read speed (much slower then write speed)

#define ILI9341_T4_DEFAULT_VSYNC_SPACING 2           // vsync on with framerate = refreshrate/2 = 45FPS. 
#define ILI9341_T4_DEFAULT_DIFF_GAP 6                // default gap for diffs (typ. between 4 and 50)
#define ILI9341_T4_DEFAULT_LATE_START_RATIO 0.3f     // default "proportion" of the frame admissible for late frame start when using vsync. 

#define ILI9341_T4_TRANSACTION_DURATION 3           // number of pixels that could be uploaded during a typical CASET/PASET/RAWR sequence. 
#define ILI9341_T4_RETRY_INIT 5                     // number of times we try initialization in begin() before returning an error. 
#define ILI9341_T4_TFTWIDTH 240                     // screen dimension x (in default orientation 0)
#define ILI9341_T4_TFTHEIGHT 320                    // screen dimension y (in default orientation 0)
#define ILI9341_T4_NB_SCANLINES ILI9341_T4_TFTHEIGHT// scanlines are mapped to the screen height
#define ILI9341_T4_MIN_WAIT_TIME  300               // minimum waiting time (in us) before drawing again when catching up with the scanline

#define ILI9341_T4_NB_PIXELS (ILI9341_T4_TFTWIDTH * ILI9341_T4_TFTHEIGHT)   // total number of pixels

#define ILI9341_T4_MAX_VSYNC_SPACING 5              // maximum number of screen refresh between frames (for sync clock stability). 
#define ILI9341_T4_IRQ_PRIORITY 128                 // priority at which we run the irqs (dma, pit timer and spi interrupts).
#define ILI9341_T4_MAX_DELAY_MICROSECONDS 1000000   // maximum waiting time (1 second)

#define ILI9341_T4_TOUCH_Z_THRESHOLD     400        // for touch
#define ILI9341_T4_TOUCH_Z_THRESHOLD_INT 75         // same as https://github.com/PaulStoffregen/XPT2046_Touchscreen/blob/master/XPT2046_Touchscreen.cpp
#define ILI9341_T4_TOUCH_MSEC_THRESHOLD  3          //

#define ILI9341_T4_SELFDIAG_OK 0xC0                 // value returned by selfDiagStatus() if everything is OK.

#define ILI9441_T4_DEFAULT_FPS_COUNTER_COLOR_FG 0xFFFF  // default values (color/opacity/position)
#define ILI9441_T4_DEFAULT_FPS_COUNTER_COLOR_BG 0x001F  // for the FPS counter
#define ILI9441_T4_DEFAULT_FPS_COUNTER_OPACITY 0.5f     // 
#define ILI9441_T4_DEFAULT_FPS_COUNTER_POSITION 0xFFFF  //


/** ILI9341 command codes */

#define ILI9341_T4_NOP 0x00
#define ILI9341_T4_SWRESET 0x01
#define ILI9341_T4_RDDID 0x04
#define ILI9341_T4_RDDST 0x09

#define ILI9341_T4_SLPIN 0x10
#define ILI9341_T4_SLPOUT 0x11
#define ILI9341_T4_PTLON 0x12
#define ILI9341_T4_NORON 0x13

#define ILI9341_T4_RDMODE 0x0A
#define ILI9341_T4_RDMADCTL 0x0B
#define ILI9341_T4_RDPIXFMT 0x0C
#define ILI9341_T4_RDIMGFMT 0x0D
#define ILI9341_T4_RDSGNMODE 0x0E
#define ILI9341_T4_RDSELFDIAG 0x0F

#define ILI9341_T4_INVOFF 0x20
#define ILI9341_T4_INVON 0x21
#define ILI9341_T4_GAMMASET 0x26
#define ILI9341_T4_DISPOFF 0x28
#define ILI9341_T4_DISPON 0x29

#define ILI9341_T4_CASET 0x2A
#define ILI9341_T4_PASET 0x2B
#define ILI9341_T4_RAMWR 0x2C
#define ILI9341_T4_RAMRD 0x2E

#define ILI9341_T4_PTLAR 0x30
#define ILI9341_T4_MADCTL 0x36
#define ILI9341_T4_VSCRSADD 0x37
#define ILI9341_T4_PIXFMT 0x3A

#define ILI9341_T4_FRMCTR1 0xB1
#define ILI9341_T4_FRMCTR2 0xB2
#define ILI9341_T4_FRMCTR3 0xB3
#define ILI9341_T4_INVCTR 0xB4
#define ILI9341_T4_DFUNCTR 0xB6

#define ILI9341_T4_PWCTR1 0xC0
#define ILI9341_T4_PWCTR2 0xC1
#define ILI9341_T4_PWCTR3 0xC2
#define ILI9341_T4_PWCTR4 0xC3
#define ILI9341_T4_PWCTR5 0xC4
#define ILI9341_T4_VMCTR1 0xC5
#define ILI9341_T4_VMCTR2 0xC7

#define ILI9341_T4_RDID1 0xDA
#define ILI9341_T4_RDID2 0xDB
#define ILI9341_T4_RDID3 0xDC
#define ILI9341_T4_RDID4 0xDD

#define ILI9341_T4_GMCTRP1 0xE0
#define ILI9341_T4_GMCTRN1 0xE1
#define ILI9341_T4_PWCTR6  0xFC



#define ILI9341_T4_ALWAYS_INLINE __attribute__((always_inline))






/*************************************************************************************************************
* ILI9341 screen driver for Teensy 4/4.1.
*
* The driver performs fast blitting of a memory framebuffer onto the screen using SPI transfer (synchronous
* or asynchronous with DMA) with the following additional features:
*
* (1) Implements partial redraw where only the pixels that changes between frames are updated.
*
* (2) Implements 'vsync' to prevent screen tearing (pixels are updated behind the refresh scanline).
*
* (3) Adjustable screen refresh rate (40 - 120hz) and framerate (asap or locked with the refresh rate).
*
* (4) Multiple buffering mode: no buffering / double buffering 
*
* (5) The object can also manage a XPT2046 touchscreen on the same SPI bus.
*
*
* The memory framebuffers should have the usual layout:
*
*                   SCREEN PIXEL(i,j) = framebuffer[i + (width * j)]
*
* for 0 <= i < width  and  0 <= j < height
* 
* with the 'width' and 'height' value depending on the chosen screen orientation and where the pixels color are
* given in uint16_t RGB565 format.
* 
*RGB565 layout as uint16:     bit 15            bit 0
*                                  RRRRR GGGGGG BBBBB
* 
*    -> red bits   [15-11] 
*    -> green bits [10-5]
*    -> blue bits  [4-0]
*
****************************************************************************************************************/
class ILI9341Driver
{

public:




    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Initialization and general settings
    *
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * Constructor. Set the hardware pins but do not initialize anything yet.
    * 
    * Pin not set are given default value 255. 
    *
    * - cs :  Pin connected to the CS pin on the display. Mandatory if present.
    *         Can be any digital pin on the teensy: it does not need to be an hardware accelerated CCS pin...
    *         --> Some display do not have a CS pin, in the case, set cs=255. 
    *        
    * - dc :  Pin connected to the DC pin on the display. Mandatory.  
    *         Can be any digital pin on the teensy BUT USING A HARDWARE ACCELERATED CS PIN FOR DC WILL PROVIDE
    *         A IMPORTANT SPEEDUP !!! 
    * 
    * - sclk: Pin connected to the SCK pin on the display. Mandatory.   
    *         Must be a hardware SCK pin for the SPI bus used on the Teensy. 
    *         
    * - mosi: Pin connected to the SDI(MOSI) pin on the display. Mandatory.   
    *         Must be a hardware MOSI pin for the SPI bus used on the Teensy.
    *
    * - miso: Pin connected to the SDO(MISO) pin on the display. Mandatory.   
    *         Must be a hardware MISO pin for the SPI bus used on the Teensy.
    *
    * - rst:  Pin connected to the RESET pin on the display. Optional but recommended.   
    *         -> if not connected to the teensy, set rst=255 and do not forget the pull 
    *         the RST pin to +33V the display
    * 
    * If the screen has an XPT2046 touchscreen ON THE SAME SPI BUS:
    * 
    * - touch_cs : Pin connected to the T_CS pin on the display. Mandatory to use the touchscreen.
    *              Can be any digital pin.
    * 
    * - touch_irq: Pin connected to the T_IRQ pin on the display. Optional. 
    *              Can be any digital pin.
    *              
    *  --> When using the touchscreen, the T_CLK, T_DIN and T_DO pin fron the screen must be connected
    *      to the same pin as SCK, SDI and SDO  (i.e. the screen and touchscreen share the same SPI bus)
    *      
    * --------------------------------------------------------------------------------------------
    * THE SPI BUS SHOULD BE DEDICATED TO THE SCREEN (AND POSSIBLY THE XPT2046 TOUCHSCREEN)
    * DO NOT CONNECT ANY OTHER DEVICE ONT THAT SPI BUS !!!
    * --------------------------------------------------------------------------------------------
    * 
    **/
    ILI9341Driver(uint8_t cs, uint8_t dc, uint8_t sclk, uint8_t mosi, uint8_t miso, uint8_t rst = 255, uint8_t touch_cs = 255, uint8_t touch_irq = 255);




    /**
    * Set the output Stream used by the driver for displaying infos. 
    *
    * Set this to nullptr to prevent any output.debugging.
    * 
    * The stream is used, in particular by the following methods:
    * 
    * - `begin()` for debugging/displaying debug information.
    * - `printStatus()` to check the the driver status.    
    * - `printRefreshMode()` to display available refresh rates/modes.
    * - `printStats()` to print statistics about frame uploads.
    * - `calibrateTouch()` to provide instruction for calibration.
    * 
    * Typical usage: output(&Serial)
    **/
    void output(Stream * outputStream = nullptr) { _outputStream = outputStream; }


    /**
    * Initialize the screen (and optionally set the speed for read/write SPI transfers).
    *
    * THis method should be called only once. There is no associated end() method.
    * 
    * Return true if initialization is OK, false if an error occurred.
    * 
    * NOTE: if an output stream is set with the `output()` method, then debug information
    *       are sent to this stream.
    **/
    bool begin(uint32_t spi_clock = ILI9341_T4_DEFAULT_SPICLOCK, uint32_t spi_clock_read = ILI9341_T4_DEFAULT_SPICLOCK_READ);


    /**
    * Query the value of the self-diagnostic register.
    * 
    * Should return ILI9341_T4_SELFDIAG_OK = 0xC0 if everything is fine.
    **/
    int selfDiagStatus();


    /**
    * Print some info about the screen status (for debug purpose).
    * 
    * Output is sent to the stream set with the `output()` method.
    * 
    * Use printStats() instead to get statistics for optimization purposes. 
    **/
    void printStatus();


    /**
    * Set the speed for subsequent SPI writes.
    *
    * Remark: calling this method reset all statistics.
    **/
    void setSpiClock(int spi_clock = ILI9341_T4_DEFAULT_SPICLOCK);


    /**
    * Query the spi speed for SPI writes.
    **/
    int  getSpiClock() const { return _spi_clock; }


    /**
    * Set the speed for subsequent SPI reads.
    *
    * Remark: calling this method reset the statistics.
    **/
    void setSpiClockRead(int spi_clock = ILI9341_T4_DEFAULT_SPICLOCK_READ);
    


    /**
    * Query the spi speed for SPI reads.
    **/
    int  getSpiClockRead() const { return _spi_clock_read; }



    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Misc commands
    *
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * Enter/exit sleep mode.
    **/
    void sleep(bool enable);


    /**
    * Invert the display colors.
    **/
    void invertDisplay(bool i);


    /**
    * Set the vertical scroll offset.
    *
    * Default value is 0 (no scroll). When an offset is set, the framebuffer is shifted vertically on 
    * the screen by the given offset. This means that the following (hardware) mapping is performed:
    * 
    * - framebuffer line i  =>  drawn at scanline (i - offset) mod TFT_HEIGHT. 
    * 
    * offset can be any value (positive or negative) so that incrementing / decrementing it enables
    * to scroll up or down continuously.
    **/
    void setScroll(int offset = 0);
       



    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Screen Orientation 
    *
    * -> these methods determine the screen orientation which affects the framebuffer dimensions and its 
    *    layout.
    *    
    ****************************************************************************************************
    ****************************************************************************************************/


    /** The 4 possible orientations */
    enum
        {
        PORTRAIT_240x320            = 0,
        LANDSCAPE_320x240           = 1,
        PORTRAIT_240x320_FLIPPED    = 2,
        LANDSCAPE_320x240_FLIPPED   = 3,
        };



    /**
    * Set the screen Orientation (between 0 and 3)
    *
    * The default start up orientation is 0.
    * 
    * The framebuffer layout depend on whether the display is in portrait or landscape mode.
    * 
    * - orientation 0 and 2 in portrait mode : 240x320.
    * - orientation 1 and 3 in landscape mode : 320x240.
    *
    * NOTE: Orientation 0 the the only one for with pixels refresh order coincide with the framebuffer
    * ordering. Using this orientation may provide a marginal increase in the driver speed.
    * 
    * Remark: calling this method resets the statistics (if the orientation changes). 
    **/
    void setRotation(uint8_t r);


    /**
    * Return the current screen orientation (between 0 and 3)
    *
    * The default start up orientation is 0.
    * 
    * The framebuffer layout depends on whether the display is in portrait or landscape mode:
    * 
    *  SCREEN_PIXEL(i,j) = framebuffer[i + width*j]
    *  
    * with width =  240 if orientation is 0 or 2
    *            =  320 if orientation is 1 or 3.
    **/
    int getRotation() const { return _rotation; }


    /**
    * Return the screen witdh (w.r.t the current orientation).
    **/
    int width() const { return _width; }


    /**
    * Return the screen height (w.r.t the current orientation).
    **/
    int height() const { return _height; }





    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Screen refresh rate. 
    * 
    * -> these methods are used to set the screen refresh rate (number of time the display is refreshed 
    *    per second). This rate is important because it is related to the actual framerate via the 
    *    vsync_spacing parameter (c.f. the vsync setting section). 
    *
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * set the refresh mode between 0 and 31.
    *
    * - 0  : fastest refresh rate (around than 120/140hz). 
    * - 31 : slowest refresh rate (around 30/40hz).
    * 
    * NOTE: the exact refresh rate for a given mode varies from display to display. 
    *       Once the mode set, use getRefreshRate() to find out the refresh rate.
    *
    * By default the refresh mode selected is 0 (fastest possible). 
    * 
    * Remark: calling this method resets the statistics.
    **/
    void  setRefreshMode(int mode);


    /**
    * Return the current refresh mode. 
    *
    * - 0  : fastest refresh rate (around than 120/140Hz). 
    * - 31 : slowest refresh rate (around 30/40Hz).   
    *
    **/
    int getRefreshMode() const { return _refreshmode; }


    /**
    * Set the refresh mode for the display to match the requested refresh rate (in Hz). 
    * as close as possible. 
    * 
    * After this method returns. Use getRefreshMode() and getRefreshRate() to find
    * out the actual mode and exact refresh rate set. Note that these values will varies
    * from display to display. 
    *
    * Remark: calling this method resets the statistics.
    **/
    void setRefreshRate(float refreshrate_hz);


    /**
    * Get the refresh rate of the display (in Hz) corresponding to the current
    * refresh mode set.
    **/
    float getRefreshRate() const { return (_period == 0) ? 0.0f : 1000000.0f / _period; }


    /**
    * Display all the possible screen refresh mode with the corresponding refresh rates. 
    * 
    * The infos are sent to the output stream set with the `output()` method.
    * 
    * This method is will take a few seconds as its cycles through all the modes
    * and must sample the exact refresh rate each time.
    **/
    void printRefreshMode();





    /***************************************************************************************************
    ****************************************************************************************************
    *
    * VSync settings and framerate locking. 
    *
    * -> these methods decide how updates are carried out and whether operations are synced with the 
    *    display refresh rate or, on the contrary, are pushed as fast as possible. 
    *
    ****************************************************************************************************
    ****************************************************************************************************/



    /**
    * This parameter defines the upload strategy and determines how the actual framerate relates 
    * to the display refresh rate. 
    *
    * This number must be between -1 and 5 = ILI9341_T4_MAX_VSYNC_SPACING.
    *
    * - vsync_spacing = -1. In this case, screen updates occur as soon as possible and some frames 
    *                       may even be dropped when using double buffering if they are pushed to 
    *                       update() faster than they can be uploaded to the screen. This mode will 
    *                       provide the fastest 'apparent' framerate but at the expanse of picture 
    *                       quality and will also induce screen tearing (no VSync)
    *                       This mode also does not insure any framerate control/stability. 
    *
    * - vsync_spacing = 0. Again, screen updates are pushed to the screen as fast as possible but 
    *                      frames are not dropped so the driver will wait if an update is already 
    *                      in progress. 
    *                      Again, this mode provides no guaranty concerning screen tearing nor 
    *                      framerate stability. 
    *
    * - vsync_spacing > 0. The updates are synchronized with the screen refresh. This approach has 
    *                      two advantages:
    *                      1) It prevents screen tearing.
    *                      2) It insure a constant framerate.
    *
    *                      vsync_spacing is the number of screen refresh between two consecutive 
    *                      updates. Thus, the real framerate is given by:
    *
    *                      real_framerate = screen_refresh_rate / vsync_spacing
    *
    *                      vsync_spacing should be set to either:                           
    *                      1 (framerate = refresh_rate)  or 2 (framerate = half refresh rate).   
    *                       
    *                      --> Larger values are not really useful except for special cases. 
    *
    * NOTE 1: In order to insure that screen tearing cannot occur, the upload must be done fast 
    *         enough so that the refresh 'scanline' does not catch up with the pixels currently
    *         being updated. This is possible if the upload takes less than 2 refresh periods.
    *         For this reason, vsync_spacing = 2  is the optimal choice in most case. However, 
    *         if the diff is simple enough so that the frame upload rate is faster than the 
    *         screen refresh rate, then it may be worth setting vsync_spacing = 1 ...
    *
    * ADVICE : USE vsync_spacing = 2 AND ADJUST THE DISPLAY REFRESH RATE WITH setRefreshRate() 
    *          TO GET A CONSISTENT FRAMERATE WITHOUT SCREEN TEARING
    * 
    * Remark: calling this method resets the statistics.
    **/
    void setVSyncSpacing(int vsync_spacing = ILI9341_T4_DEFAULT_VSYNC_SPACING);


    /**
    * Return the current vsync_spacing parameter. 
    **/
    int getVSyncSpacing() const { return _vsync_spacing;  }



    /**
    * Set how late we can be behind the scan line and still start uploading a frame without
    * waiting for the next refresh for synchronization. Set a value in [0.1f, 0.9f]
    *
    * - Choosing a small value will reduce screen tearing but may make the framerate oscillate more
    *   when the timing is tight.
    *
    * - Choosing a large value will stabilize the framerate but at the expense of more screen tearing
    *   when the timing is tight.
    *
    * Remark: calling this method resets the statistics.
    **/
    void setLateStartRatio(float ratio = ILI9341_T4_DEFAULT_LATE_START_RATIO);


    /**
    * Return the value of the "late start ratio" parameter.
    **/
    float getLateStartRatio() const { return _late_start_ratio; }


    /**
    * Force a re-synchronization with the screen scanline on the next frame upload. 
    * This command has no effect when not in vsync mode (ie when vsync_spacing <= 0).
    * 
    * This method should not be used under normal circumstance.
    **/
    void resync() { _late_start_ratio_override = true; }




    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Buffering mode 
    * 
    * -> these methods decide whether update operations are carried immediately or if an internal
    *    framebuffer is used for double buffering and asynchronous upload via DMA.
    * 
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * Set/remove the internal framebuffer.
    * 
    * The mode of operation of the update() method depends on whether an internal framebuffer is set.
    *
    * - no internal framebuffer  : all updates operations are performed immediately.
    * - internal framebuffer set : double buffering and asynchronous transfer in background via DMA.
    * 
    * ----------------------------------------------------------------------------------------------
    * THE INTERNAL FRAMEBUFFER IS GIVEN FOR INTERNAL USE BY THE CLASS AND SHOULD NOT BE MODIFIED ONCE 
    * SET (except if you know what you are doing :-))
    * ----------------------------------------------------------------------------------------------
    * 
    * Remarks: 
    * 
    * 1. Calling the method with empty parameter/nullptr removes any previously set framebuffer.  
    *  
    * 2. Calling this method resets the statistics.
    **/
    void setFramebuffer(uint16_t* fb1 = nullptr);


    /**
    * Alias for the SetFrameBuffer() method.
    * 
    * For compatibility with previous library version only: the second framebuffer fb2 is ignored. 
    **/
    void setFramebuffers(uint16_t* fb1 = nullptr, uint16_t* fb2 = nullptr) { setFramebuffer(fb1 ? fb1 : fb2); }



    /** Buffering mode*/
    enum
        {
        NO_BUFFERING = 0,
        DOUBLE_BUFFERING = 2,
        };


    /**
    * Return the current buffering mode:
    *
    * - 0 = NO_BUFFERING     : no internal framebuffer. All updates operations are carried immediately.
    * - 2 = DOUBLE_BUFFERING : internal framebuffer set. Double buffering using asynchronous transfer via DMA.
    * 
    **/
    int bufferingMode() const { return ((_fb1) ? DOUBLE_BUFFERING : NO_BUFFERING);}





    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Differential updates settings
    * 
    ****************************************************************************************************
    ****************************************************************************************************/



    /**
    * Set/remove one (or two) internal diff buffers used for performing differential updates. 
    *
    * When diff buffers are set. They will be used whenever possible to create 'diff' between the current 
    * screen content and the new framebuffer to be uploaded so that only pixels that change color will be 
    * uploaded, drastically reducing the upload time and therefore providing a boost on the effective 
    * framerate. 
    * 
    * In order to use differential update, the driver must 'know' the current screen content which means 
    * that an internal frmaebuffer must be set (ie double buffering enabled). To enable differential updates
    * you must both set at least 1 diff buffer AND the internal framebuffer. 
    * 
    * Setting a second diff buffer is optional but will increase the framerate as it allows the driver to 
    * compute the diff of the next frame while still performing the asynchronous transfer of the previous 
    * frame. 
    * 
    * ----------------------------------------------------------------------------------------------
    * ONCE SET, THE DIFFBUFFER BELONG TO THIS OBJECT AND MUST NOT BE TOUCHED FOR CREATING OR READING
    * DIFFS OR THE WHOLE PROGRAM MAY CRASH !!!
    *
    * HOWEVER, IT IS STILL POSSIBLE TO CALL ALL THE STATSXXX() METHODS OF THE DIFFS TO CHECK MEMORY
    * CONSUMPTION / CPU USAGE...
    *
    * IF YOU WANT THE DIFF BUFFER BACK, JUST CALL THE METHOD AGAIN WITH EMPTY PARAMETERS TO REMOVE THEM
    * FROM THE DRIVER
    * ----------------------------------------------------------------------------------------------
    *
    * Remark: 
    * 
    * - Calling this method resets the stats (if the buffering mode changes).  
    * - Use 2 diff buffers whenever possible !
    **/
    void setDiffBuffers(DiffBuffBase* diff1 = nullptr, DiffBuffBase* diff2 = nullptr);



    /**
    * Query whether the driver performs full screen transfer or differential updates. 
    * 
    * Differential updates are enabled as soon as the following two conditions are meet:
    * 
    * 1. An internal framebuffer has been set (ie DOUBLE_BUFFERING is active)          
    * 2. At least one diff buffer has been set. 
    * 
    * NOTE: Use 2 diff buffers whenever possible !
    **/
    bool diffUpdateActive() const { return ((bufferingMode() == DOUBLE_BUFFERING) && (_diff1 != nullptr)); }



    /**
    * Set the gap used when creating diffs. 
    * 
    * [See the DiffBuff class for more detail on the gap parameter].

    * This parameter correspond to the number of consecutive unchanged pixels needed to break a SPI 
    * transaction. A smaller value will give results in less pixels being uploaded but will, on the 
    * other hand create larger diffs... The optimal value should be between 4 and 20 and will depend 
    * on the kind of graphics drawn. 
    * 
    * As a rule of thumb:
    * 
    * - Gap larger than 10 for diff buffer with less than 4K of memory
    * - Gap between 6 to 10  for diff buffers with size between 4K to 8K.  
    * - Gap between 4 to 6 for diff buffers with size larger then 8K. 
    * 
    * You can use the printStats() to check how much memory the diff buffer typically consume. If the
    * diffs buffers overflow too often, you should either increase the gap or increase their size.
    * 
    * Remark: calling this method resets the statistics.
    **/
    void setDiffGap(int gap = ILI9341_T4_DEFAULT_DIFF_GAP);



    /**
    * Return the current gap parameter used when creating diffs. 
    **/
    int getDiffGap() const { return _diff_gap; }


    /**
    * Set the mask used when creating a diff to check is a pixel is the same in both framebuffers. 
    * If the mask set is non-zero, then only the bits set in the mask are used for the comparison 
    * so pixels with different values may be considered equal and may not redrawn.
    * 
    * Setting a mask may be useful when the framebuffer being uploaded to the screen comes from
    * a camera or another source that introduces random noise that would prevent the diff from
    * finding large region of identical pixels (hence making the diff pretty useless) but when 
    * it does not really matter to have a 'perfect' copy of the framebuffer on the screen. 
    * 
    * Typically, one wants to set the lower bits on each channel color to 0 so that color that
    * are 'close' are not always redrawn (see the other method version below). 
    * 
    * If called without argument, the compare mask is set to 0 hence disabled and strict equality
    * is enforced when creating diffs (default behavior).
    * 
    * -> RGB565 layout as uint16: bit 15            bit 0
    *                                  RRRRR GGGGGG BBBBB
    **/
    void setDiffCompareMask(uint16_t mask = 0);


    /**
    * Set the compare mask by specifying for each color channel the number of lower bits
    * that should be ignored. 
    * 
    * Recall that there are 5 bits for the blue and red channel and 6 bits for the green
    * channel. 
    * 
    * -> RGB565 layout as uint16: bit 15            bit 0
    *                                  RRRRR GGGGGG BBBBB
    **/
    void setDiffCompareMask(int bitskip_red, int bitskip_green, int bitskip_blue);


    /**
    * Return the value of the current compare_mask. 
    * 
    * Return 0 if the mask is not set and strict comparison of pixel colors is enforced 
    * (which is the default value).
    **/
    uint16_t getCompareMask() const { return _compare_mask; }




    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Screen updates
    *
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * Clear the screen to a single color (default black). 
    *
    * This operation is done immediately (i.e. not async) so the screen is cleared on return.
    **/
    void clear(uint16_t color = 0);



    /**
    *                                 MAIN SCREEN UPDATE METHOD
    *
    * Push a framebuffer to be displayed on the screen. The behavior of the method depend on the
    * current buffering mode and the vsync_spacing parameter.
    *
    * - fb : the framebuffer to draw unto the screen.     
    *  
    * - force_full_redraw: If set to true, then differential update is disabled for this particular   
    *                      frame and the whole screen is updated (even when a diff could have been used). 
    *                      Normally, this option should not be needed except in the very special cases 
    *                      where one knows for sure that the diff will be useless so disabling it saves 
    *                      some CPU times that would have been used for creating the diff (around 1us 
    *                      normally).
    *
    * WHEN THE METHOD RETURNS, THE FRAME MAY OR MAY NOT ALREADY BE DISPLAYED ONT THE SCREEN BUT
    * THE INPUT FRAMEBUFFER fb CAN STILL BE REUSED IMMEDIATELY IN ANY CASE. (when using async updates,
    * the internal framebuffer is used to save a copy of the framebuffer).
    *
    * 
    * The exact behavior of the method depends on bufferingMode():
    *
    * 
    * -> NO_BUFFERING (i.e. no internal framebuffer set):
    *
    *   The framebuffer is pushed to the screen immediately and the method returns only when upload is 
    *   complete. In this mode, differential upload are always disabled: the whole screen is updated
    *   and diff buffers, if present, are ignored. 
    *
    *   - if vsync_spacing <= 0, upload to the screen start immediately (no VSync).
    *
    *   - if vsync_spacing >= 1, screen upload is synchronized with the screen refresh (VSync) and the
    *     method waits until vsync_spacing refreshes have occurred since the previous update to insure
    *     a constant framerate equal to (refresh_rate/vsync_spacing).
    * 
    *
    * -> DOUBLE_BUFFERING (internal framebuffer set)
    *
    *   All updates are done asynchronously via DMA and the method returns asap. If 1 or 2 diff buffers
    *   are present, they are automatically used to perform differential update: only the portion of
    *   the screen whose content has changed is redrawn. 
    *
    *   - if vsync_spacing = -1, upload to the screen starts immediately unless there is already a
    *     transfer in progress in which case the frame is simply dropped and the method return without
    *     doing anything (no VSync).
    *
    *   - if vsync_spacing = 0, upload to the screen start immediately unless there is already a
    *     transfer in progress in which case the method waits until the transfer completes and then 
    *     starts another async transfer immediately (no VSync).
    *
    *   - if vsync_spacing > 0. screen upload is synchronized with the screen refresh and the
    *     method waits until vsync_spacing refreshes have occurred since the previous update to
    *     insure a constant framerate equal to (refresh_rate/vsync_spacing). If a transfer is
    *     already in progress, it waits for it to complete before scheduling the next transfer
    *     via DMA and returning.
    * 
    *
    * NOTE: 
    *       (1) double buffering give a HUGE improvement over the no buffering method at the
    *           expense of an additional internal memory framebuffer (150Kb).
    *
    *       (2) Setting two diffs buffers instead of one cost only a few additional kilobytes
    *           yet will usually improve the max framerate significantly since it enables the
    *           driver to compute the next diff while the previous update is still ongoing.
    *        
    *              
    * ADVICE:  US AN INTERNAL FRAMEBUFFER + 2 DIFF BUFFERS (WITH SIZE RANGING FROM 5K TO 10K).
    * 
    **/
    void update(const uint16_t* fb, bool force_full_redraw = false);



    /**
    *                             PARTIAL SCREEN UPDATE METHOD
    *
    * Update only part of the screen. The behavior of the method depend on the current buffering 
    * mode and the vsync_spacing parameter.
    *
    * WHEN THE METHOD RETURNS, THE FRAME MAY OR MAY NOT ALREADY BE DISPLAYED ON THE SCREEN BUT
    * THE INPUT FRAMEBUFFER fb CAN STILL BE REUSED IMMEDIATELY IN ANY CASE (A COPY IS MADE WHEN
    * USING ASYNC UPDATES).
    *
    * Parameters:
    * 
    * - fb : framebuffer to the rectangular region to update.     
    * 
    * - [xmin, xmax] x [ymin, ymax] : region of the screen to update  
    * 
    * - stride : stride for the supplied framebuffer fb
    *
    *            The layout of fb is such that, 
    *   
    *                 screen pixel(xmin + i, ymin + j) = fb[i + stride*j]
    *                 
    *            -> If stride is not specified, it defaults to (xmax - xmin + 1) which is the
    *                width of the rectangular region.
    *    
    * - redrawNow: - If set to true, the screen is redrawn immediately (async if an internal   
    * -              framebuffer is set). 
    *              - If set to false and an internal framebuffer is available, then the changes   
    *                are stored in the internal framebuffer but are not drawn on the screen. 
    *                This permits to call regionUpdate() several times without drawing onto the 
    *                screen and then draw all the changes simultaneously when needed. This is 
    *                particularly convenient when using the lvgl library. 
    *
    *
    * NOTE: (1) Similarly to the 'update()' method, this method will use VSync when enabled
    *           depending on the value of the vsync_spacing parameter 
    *
    *       (2) In there is no internal buffer, then screen is updated immediately even if
    *           redrawNow=false.  
    *
    *       (3) For this method, TWO DIFF BUFFERS ARE REQUIRED FOR DIFFERENTIAL UPDATE !
    *           Setting only one diff buffer will disable differential updates :(
    *
    **/    
    void updateRegion(bool redrawNow, const uint16_t* fb, int xmin, int xmax, int ymin, int ymax, int stride = -1); 



    /**
    * Wait until any currently ongoing async update completes.
    * 
    * Returns immediately if no update is ongoing.
    * 
    * NOTE: This method should not be called normally it will create a 
            "busy wait" and wastes precious CPU time...
    **/
    void waitUpdateAsyncComplete() { _waitUpdateAsyncComplete(); }



    /**
    * Return true if an Async update is currently ongoing and false otherwise.
    **/
    bool asyncUpdateActive() const { return (_dma_state != ILI9341_T4_DMA_IDLE); }





    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Statistics
    * 
    * The driver monitor many stats which are useful for performance optimization. Two methods are 
    * particularly useful:
    * 
    * - overlayFPS() : add an FPS counter on a corner of the screen
    * - printStats() : Print out all the statistics on the output stream.     
    * 
    * Note: Some methods return a 'StatsVar' object which is a class that holds stats about a sequence 
    *       of values: c.f. StatsVar.h for more info. 
    *       
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * Draw the FPS counter on a corner of the supplied framebuffer with given colors 
    * and opacity.
    * 
    * The FPS displayed correspond to the number of frames uploaded by the driver 
    * during the last second.
    * 
    * Call this method just before update() to display the instantaneous framerate. 
    * 
    * - fb : the framebuffer to draw onto
    * - position: position of the counter on the framebuffer:
    *             0= top right,  1=bottom right,  3=bottom left,  4=top left
    * - fg_color : foreground color.   
    * - bg_color : background color  
    * - opacity : opacity of the counter between 0.0f=transparent and 1.0f=opaque. 
    **/
    void overlayFPS(uint16_t* fb, 
                    int position = ILI9441_T4_DEFAULT_FPS_COUNTER_POSITION, 
                    uint16_t fg_color = ILI9441_T4_DEFAULT_FPS_COUNTER_COLOR_FG, 
                    uint16_t bk_color = ILI9441_T4_DEFAULT_FPS_COUNTER_COLOR_BG, 
                    float opacity = ILI9441_T4_DEFAULT_FPS_COUNTER_OPACITY);


    /**
    * Reset all statistics
    **/
    void statsReset(); 


    /**
    * Return the number of frames drawn since the last call to statReset().
    **/
    uint32_t statsNbFrames() const { return _stats_nb_frame; }


    /**
    * Return the number of milliseconds since the last call to statReset().
    **/
    uint32_t statsTotalTime() const { return _stats_elapsed_total; }


    /**
    * Return the 'average' framerate in Hz which is simply computed as the 
    * total number of frame drawn divided the total time since the last reset. 
    **/
    float statsFramerate() const { return  (_stats_nb_frame == 0) ? 0.0f : ((_stats_nb_frame * 1000.0f) / _stats_elapsed_total); }


    /**
    * Return the instantaneous FPS i.e. the number of frames displayed during
    * the previous second. This is the same value that is drawn with the
    * overlayFPS() method. 
    **/
    uint32_t statsCurrentFPS() const { return _stats_current_fps;  }


    /**
    * Return an object with detailed statistics about the instantaneous FPS.
    **/
    StatsVar statsFPS() const { return _statsvar_fps; }


    /**
    * Return an object containing statistics about the CPU time
    * used spend preparing and updating the screen (dma interrupt time). 
    * 
    * -> This does NOT count the time needed to create the diffs which
    *    can be queried separately directly from the diff buffers. 
    **/
    StatsVar statsCPUtimePerFrame() const { return _statsvar_cputime; }


    /**
    * Return an object containing statistics about the time taken
    * for uploading each frame. 
    * 
    * -> When using async update, this time incorporate the 'DMA time'
    *    where the CPU remains available. 
    *
    * -> This does NOT count the time needed to create the diffs which
    *    can be queried separately directly from the diff buffers.
    **/
    StatsVar statsUploadtimePerFrame() const { return _statsvar_uploadtime; }


    /**
    * Return an object containing statistics about the number of pixels
    * uploaded per frame. 
    **/
    StatsVar statsPixelsPerFrame() const { return _statsvar_uploaded_pixels; }


    /**
    * Return the ratio of the average number of pixels uploaded per frame 
    * compared to the total number of pixel on the screen.
    **/
    float statsRatioPixelPerFrame() const  { return (((float)_statsvar_uploaded_pixels.avg()) / ILI9341_T4_NB_PIXELS); }


    /**
    * Return an object containing statistics about the number of transactions
    * per frame (i.e. roughly the number of pieces in the diff). 
    **/
    StatsVar statsTransactionsPerFrame() const { return _statsvar_transactions; }


    /**
    * Return an estimate of the speed-up obtained by using the differential updates 
    * compared to full updates. This estimate is only about the time needed to upload 
    * the pixels without taking VSync into account. 
    * 
    * If the value returned is smaller than one. It means that there is not real 
    * benefits to using differential updates and so it should be disabled.
    **/
    float statsDiffSpeedUp() const { return ((!diffUpdateActive())|| (_statsvar_transactions.count() == 0)) ? 1.0f : (((float)(ILI9341_T4_NB_PIXELS * 16)) / ((float)_spi_clock) * (1000000.0f / _statsvar_uploadtime.avg())); }


    /**
    * Return an object containing statistics about the "margin" during upload a vsynced frame.
    * 
    * The margin of a VSynced uploaded frame is minimum difference during the upload between 
    * the position of the pixels being uploaded and the screen scanline currently being refreshed.
    *
    * When this value becomes negative, it means screen tearing occurs and the SPI speed should be
    * increased (or the refreshrate decreased). On the other hand, a large positive value means 
    * that there is plenty of time for redraw without tearing :-)
    **/
    StatsVar statsMarginPerFrame() const { return _statsvar_margin; }


    /**
    * Return an object containing the effective statistics about the vsync_spacing between screen 
    * refresh. If the framerate is stable this value should be close to the requested value and
    * the standard deviation (std) should be small.
    **/
    StatsVar statsRealVSyncSpacing() const { return _statsvar_vsyncspacing; }


    /**
    * Return the number of frames with VSync active for which screen tearing (may) have occurred.
    **/
    uint32_t statsNbTeared() const { return _nbteared; }


    /**
    * Return the ratio of frames with VSync active for which screen tearing may have occurred.
    * (returns 1.0f when VSync is disabled).  
    **/
    float statsRatioTeared() const { return (_vsync_spacing <= 0) ? 1.0f : ((_statsvar_vsyncspacing.count() == 0) ? 0.0f : (((float)_nbteared) / _statsvar_margin.count())); }


    /**
    * Output many statistics about the object into a stream.
    * 
    * The output stream set with the `output()` method is used.
    *
    * This method is very useful for fine-tuning the parameters and checking
    * that the parameters (gap, vsync_spacing, refresh rate, diff buffer size...) 
    * are correctly set. 
    * 
    * - diff_stats : true to print also the stats about the diff buffer in use.
    **/
    void printStats(bool diff_stats = true) const;





    /***************************************************************************************************
    ****************************************************************************************************
    *
    * Touch screen.
    *
    * These methods are available only if the XPT2046 touchscreen is on the same SPI bus as the screen
    * and the touch_cs (and optionally _touch_irq) pin have been assigned in the constructor.
    *
    ****************************************************************************************************
    ****************************************************************************************************/


    /**
    * Return the number of milliseconds since the touch interrupt was last triggered or -1 if no touch
    * interrupt occurred since the last call to lastTouched().
    *
    * If the touch_irq pin is not assigned, this method always return -1;
    *
    * The benefit of lastTouched() over readTouch() is that it never uses the SPI bus (and thus always
    * returns immediately).
    **/
    int32_t lastTouched();


    /**
    * Read the touchscreen. Return the position (x,y) and pressure level z.
    * Return true if the screen is being touched.
    * 
    * The coord. (x,y) returned are given w.r.t. the current screen orientation if calibration
    * data are loaded but are 'raw' value (independent of orientation) is no calibration data
    * is currently loaded. 
    * 
    * If the touch_irq pin is assigned, the method will avoid using the SPI bus whenever possible.
    *
    * If the SPI bus must be used. The method will wait until the current ongoing transfer
    * completes (if any). This means that this method may stall for a few milliseconds. 
    **/
    bool readTouch(int& x, int& y, int& z);


    /**
    * Set a mapping from touch coordinates to screen coordinates (or 
    * remove an existing mapping by calling with nullptr).
    *
    * - Until this method is called (or after being called with nullptr),
    *   readTouch() will returns the "raw" values for the (x,y) coordinate.
    *
    * - Once calibration has been set, readTouch() will subsequently return   
    *   the (x,y) coordinates according to the current orientation.
    *   
    * - 'touchCalibration' is a set of 4 values corresponding to touch values   
    *    for positions {x[0], x[239], y[0], y[319]} in orientation 0. This is 
    *    the same array as returned by calibrateTouch();
    **/
    void setTouchCalibration(int touchCalibration[4] = nullptr);


    /**
    * Query the current calibration data loaded for the touchscreen. 
    * 
    * Return true and put the data in 'touchCalibration' if available.
    * Otherwise, return false and do not modify the touchCalibration
    * array.
    **/
    bool getTouchCalibration(int touchCalibration[4]);


    /**
    * Perform interactive touchscreen calibration. 
    * 
    * Instructions for calibration are given on screen but also 
    * on the output stream set with the `output()` method.
    *  
    * - After the method returns, calibration data are also loaded 
    *   onto the driver and can be queried with getTouchCalibration(). 
    * 
    * - if touchCalibration[4] supplied is not null, the calibration   
    *   data are also copied into that array upon return of the method.
    **/
    void calibrateTouch(int touchCalibration[4] = nullptr);


    /**
    * Set the threshold value for detecting a touch event. 
    * 
    * The default value should be good enough in most cases.
    **/
    void setTouchThreshold(int Zthreshold = ILI9341_T4_TOUCH_Z_THRESHOLD) { _touch_z_threshold = Zthreshold; }


    /**
    * Return the current threshold value for touch events.
    **/
    int getTouchThreshold() const { return _touch_z_threshold; }








private:




    /**********************************************************************************************************
    *
    * You shall go no further.
    * 
    * Hey ! This is private ! don't look !
    *
    ***********************************************************************************************************/



    /**********************************************************************************************************
    * General settings.
    ***********************************************************************************************************/

    typedef void (*callback_t)(void*);                  // function callback signature 
    using methodCB_t = void (ILI9341Driver::*)(void);   // typedef to method callback. 

    int16_t _width, _height;                    // Display w/h as modified by current rotation    
    int     _rotation;                          // current screen orientation
    int     _refreshmode;                       // refresh mode (between 0 = fastest refresh rate and 15 = slowest refresh rate). 
    
    mutable Stream * _outputStream;                      // output stream used for debugging

    /** helper methods for writing to _outputStream (without using variadic parameters...) */
    template<typename T> void _print(const T & v) const { if (_outputStream) _outputStream->print(v); }

    template<typename T> void _println(const T & v) const { if (_outputStream) _outputStream->println(v); }

    template<typename T1, typename T2> void _print(const T1 & u, const T2 & v) const { if (_outputStream) _outputStream->print(u,v); }

    template<typename T1, typename T2> void _println(const T1 & u, const T2 & v) const { if (_outputStream) _outputStream->println(u,v); }

    template<typename T1> void _printf(const char * str, const T1 & a) const { if (_outputStream) _outputStream->printf(str, a); }

    template<typename T1, typename T2> void _printf(const char * str, const T1 & a, const T2 & b) const { if (_outputStream) _outputStream->printf(str, a,b); }

    template<typename T1, typename T2, typename T3> void _printf(const char * str, const T1 & a, const T2 & b, const T3 & c) const { if (_outputStream) _outputStream->printf(str, a,b,c); }

    template<typename T1, typename T2, typename T3, typename T4> void _printf(const char * str, const T1 & a, const T2 & b, const T3 & c, const T4 & d) const { if (_outputStream) _outputStream->printf(str, a,b,c,d); }



    /**********************************************************************************************************
    * About buffering / update mode.
    ***********************************************************************************************************/

    volatile int _diff_gap;                     // gap when creating diffs.
    volatile int _vsync_spacing;                // update stategy / framerate divider. 
    volatile float _late_start_ratio;          // late start parameter (by how much we can miss the first sync line and still start the frame without waiting for the next refresh).
    volatile bool _late_start_ratio_override;   // if true the next frame upload will wait for the scanline to start a next frame. 
    volatile uint16_t _compare_mask;             // the compare mask used to compare pixels when doing a diff

    DiffBuffBase* volatile  _diff1;             // first diff buffer
    DiffBuffBase* volatile  _diff2;             // second diff buffer (if non null, then _diff1 is also non zero). 
    DiffBuffDummy* volatile _dummydiff1;        // fake diff buffer used for complete refresh and when buffering is disabled. 
    DiffBuffDummy* volatile _dummydiff2;        // fake diff buffer used for complete refresh and when buffering is disabled. 

    DiffBuffDummy _dd1, _dd2;                    // the dummy diff themselves. 

    uint16_t* volatile _fb1;                    // first internal framebuffer
    uint16_t* volatile _mirrorfb;               // framebuffer that currently mirrors the screen (or will mirror it when upload completes).
    DiffBuffBase* volatile _ongoingDiff;        // should be nullptr when mirror_fb = true.
                                                // when _mirrorfb = false, if this is not equal to nullptr, then this means that
                                                // the diff pointed here contains the difference between _fb1 and the screen
    

    /**
    * Update part of the screen using a diff buffer object representing the changes between
    * the old framebuffer and the new one 'fb'.
    * - return only when update completed.
    * - uses the _vsync_spacing parameter to choose the vsync stategy.
    **/
    void _updateNow(const uint16_t* fb, DiffBuffBase* diff);


    /**
    * Update a rectangular region of the screen directly.
    * no diff buffer (the whole region is updated)
    * no vsync (i.e. as fast as possible)
    * no dma.
    **/
    void _updateRectNow(const uint16_t* sub_fb, int xmin, int xmax, int ymin, int ymax, int stride);

    void _pushRect(uint16_t color, int xmin, int xmax, int ymin, int ymax);

    void _pushpixels(const uint16_t* fb, int x, int y, int len)  ILI9341_T4_ALWAYS_INLINE
        {
        switch (_rotation)
            {
            case 0: _pushpixels_mode0(fb, x, y, len); return;
            case 1: _pushpixels_mode1(fb, x, y, len); return;
            case 2: _pushpixels_mode2(fb, x, y, len); return;
            case 3: _pushpixels_mode3(fb, x, y, len); return;
            }
        // hum...
        }

    void _pushpixels_mode0(const uint16_t* fb, int x, int y, int len);

    void _pushpixels_mode1(const uint16_t* fb, int x, int y, int len);

    void _pushpixels_mode2(const uint16_t* fb, int x, int y, int len);

    void _pushpixels_mode3(const uint16_t* fb, int x, int y, int len);



    /**
    * Wait until transfer is complete
    **/
    void _waitUpdateAsyncComplete()
        {
        if (_dma_state != ILI9341_T4_DMA_IDLE)
            {
            elapsedMillis em = 0;
            while ((_dma_state != ILI9341_T4_DMA_IDLE))
                {
                if (em > 2)
                    { // after a few ms, we call yield in each loop.
                    yield();
                    if (em > 1000)
                        { // waiting for a full second. Really looks like its hanging. 
                        _println("ILI9341_T4Driver: Hanging in _waitUpdateAsyncComplete()...");
                        em = 0;
                        }
                    }
                }
            }
        }


    /**
    * Update part of the screen using a diff buffer object representing the changes between
    * the old framebuffer and the new one 'fb'.
    * - return asap and update is async via DMA.
    * - uses the _vsync_spacing parameter to choose the vsync stategy.
    **/
    void _updateAsync(const uint16_t* fb, DiffBuffBase* diff);



    /** clip val to [min,max] */
    template<typename T> static T _clip(T val, T min, T max)
        {
        if (val < min) val = min;
        if (val > max) val = max;
        return val;
        }


    /** swap _diff1 and _diff2 */
    void _swapdiff() { auto t = _diff1;  _diff1 = _diff2;  _diff2 = t; }


    /** swap the dummydiff */
    void _swapdummydiff() { auto t = _dummydiff1; _dummydiff1 = _dummydiff2; _dummydiff2 = t; }





    /**********************************************************************************************************
    * About DMA
    ***********************************************************************************************************/

    const uint16_t* volatile _fb;               // the framebuffer to push

    DiffBuffBase* volatile _diff;               // and corresponding diff buffer

    elapsedMicros _em_async;                    // timer for async drawing

    uint32_t _timeframestart;                   // time when drawing the last uploaded frame will start. 

    volatile uint32_t _slinitpos;               // initial scanline position
    volatile int32_t  _margin;                  // margin between scanline and redraw line. 
    volatile int      _last_y;                  // previous ending y value (for computing the margin). 
    volatile uint32_t _last_delta;              // number of refresh that occured beetween the previous two frames. 


    static ILI9341Driver* volatile _dmaObject[3];  // points back to this-> (for the corresponding spi bus)

    enum
        {
        ILI9341_T4_DMA_IDLE = 0,
        ILI9341_T4_DMA_ON = 2,
        };

    volatile uint8_t _dma_state;                // DMA current status

    DMAChannel _dmatx;                          // the dma channel object. 
  
    int                 _prev_caset_x;          // previous position set with the caset command
    int                 _prev_paset_y;          // previous position set with the paset command

    static void _dmaInterruptSPI0Diff() { if (_dmaObject[0]) { _dmaObject[0]->_dmaInterruptDiff(); } } // called when using spi 0
    static void _dmaInterruptSPI1Diff() { if (_dmaObject[1]) { _dmaObject[1]->_dmaInterruptDiff(); } } // called when using spi 1
    static void _dmaInterruptSPI2Diff() { if (_dmaObject[2]) { _dmaObject[2]->_dmaInterruptDiff(); } } // called when using spi 2

    void _dmaInterruptDiff(); // called when doing partial diff redraw


    static void _spiInterruptSPI0_software() { _dmaObject[0]->_spiInterrupt_software(); }
    static void _spiInterruptSPI1_software() { _dmaObject[1]->_spiInterrupt_software(); }
    static void _spiInterruptSPI2_software() { _dmaObject[2]->_spiInterrupt_software(); }

    // called when doing partial diff redraw
    void _spiInterrupt_software() ILI9341_T4_ALWAYS_INLINE
        {
        //noInterrupts();        // UNNEEDED ?
        _toggle(_dcport, _dcpinmask);
        if (_spi_int_phase >= 0)
            {            
            _pimxrt_spi->TDR = _spi_int_command[_spi_int_phase];
            _pimxrt_spi->TCR = (_spi_int_phase & 1) ? (_dma_spi_tcr_assert) : (_dma_spi_tcr_deassert);
            _spi_int_phase--;
            }
        else
            {      
            _pimxrt_spi->IER = 0; // disable interrupt            
            _dmatx.enable();
            }
        _pimxrt_spi->SR = 0x3f00; //Reset All flags and errors    
        asm("dsb");
        //interrupts();        // UNNEEDED ?
        }


    static void _spiInterruptSPI0_hardware() { _dmaObject[0]->_spiInterrupt_hardware(); }
    static void _spiInterruptSPI1_hardware() { _dmaObject[1]->_spiInterrupt_hardware(); }
    static void _spiInterruptSPI2_hardware() { _dmaObject[2]->_spiInterrupt_hardware(); }

    // called when doing partial diff redraw
    void _spiInterrupt_hardware() ILI9341_T4_ALWAYS_INLINE        
        {
        //noInterrupts();        // UNNEEDED ?
        _pimxrt_spi->IER = 0; // disable interrupt
        _pimxrt_spi->SR = 0x3f00; //Reset All flags and errors    
        //asm("dsb");  // not needed in this case because we disable the interrupt ? 
        _restartCpuTime();
        _stats_nb_transactions++; // count a spi transaction
        _subFrameInterruptDiff();
        _pauseCpuTime();
        //interrupts();        // UNNEEDED ?
        }


    int        _spi_int_phase;         // current phase for the spi interrupt
    uint32_t   _spi_int_command[5];    // command to execute



    /**
     * flush the cache if the array is located in DMAMEM.
     * This can take a while (100us) so don't abuse it !
     **/
    void _flush_cache(const void* ptr, size_t len) ILI9341_T4_ALWAYS_INLINE
        {
        if ((uint32_t)ptr >= 0x20200000u) arm_dcache_flush((void*)ptr, len);
        asm("dsb");
        }


    void _subFrameTimerStartcb();    // called at start of subframe

    void _subFrameTimerStartcb2();   // called at start of subframe

    void _subFrameInterruptDiff();   // called by _dmaInterruptDiff() and by timer when changing subframe.

    void _subFrameInterruptDiff2();  // called by after a pause for synchronization




    /**********************************************************************************************************
    * About timing and vsync.
    ***********************************************************************************************************/

    uint32_t _period_mode0;                     // number of microsceonds between screen refresh for the fastest mode. 
    uint32_t _period;                           // number of microsceonds between screen refresh. 
    elapsedMicros _synced_em;                   // number of microseconds sinces the last scanline synchronization
    uint32_t _synced_scanline;                  // scanline at the time of the last synchronization
    


    /**
    * wait a given number of microseconds provided the delay is smaller
    * than ILI9341_T4_MAX_DELAY_MICROSECONDS
    **/
    void _delayMicro(uint32_t t)
        {
        if (t < ILI9341_T4_MAX_DELAY_MICROSECONDS) delayMicroseconds(t); // only allow delay of at most 0.1 second. 
        }


    /**
    * Number of scanline drawn during t microseconds.
    **/
    int _nbScanlineDuring(uint32_t t) const
        {
        return ((t * ILI9341_T4_NB_SCANLINES) / _period);
        }


    /**
    * Number of microsecond to perform a given number of scanlines
    **/
    uint32_t _timeForScanlines(uint32_t nbscanline) const
        {
        return (nbscanline * _period) / ILI9341_T4_NB_SCANLINES;
        }


    /**
    * Convert a ratio in [0.0f, 1.0f] to a scanline in [|0, ILI9341_T4_NB_SCANLINES - 1|]
    **/
    int _ratioToScanline(float r) const ILI9341_T4_ALWAYS_INLINE
        {
        int l = (int)(r * ILI9341_T4_NB_SCANLINES);
        l = ILI9341Driver::_clip((int)l, (int)0, (int)ILI9341_T4_NB_SCANLINES - 1);
        return l;
        }


    /**
    * Return true is the current scanline is in the given range [|start, end|]
    **/
    bool _isScanlineInRange(int start, int end)  ILI9341_T4_ALWAYS_INLINE
        {   
        int v = _getScanLine(false);
        return ((start <= v) && (v <= end));
        }


    /**
    * Return the number of microsecond before we exit the given range.
    **/
    uint32_t _microToExitRange(int start, int end)
        {
        const int delta = end - start;
        if ((delta < 0) || ((5 * delta) >= (4 * ILI9341_T4_NB_SCANLINES))) return 0; // invalid range (negative or too big). 
        int v = _getScanLine(false);
        if ((v < start) || (v > end)) return 0; // no wait needed. 
        return 1 + _microToReachScanLine(((end + 1) % ILI9341_T4_NB_SCANLINES), false); // make sur it is not 0. 
        }


    /**
    * Return the number of microseconds remaining until we reach a given scanline.
    * WARNING: if sync = true, SPI must NOT be in use !
    **/
    uint32_t _microToReachScanLine(int scanline, bool sync) ILI9341_T4_ALWAYS_INLINE
        {
        int now = _getScanLine(sync);
        const uint32_t diff = (now <= scanline) ? (uint32_t)(scanline - now) : (uint32_t)(scanline - now + ILI9341_T4_NB_SCANLINES);
        return (diff * _period) / ILI9341_T4_NB_SCANLINES;
        }


    /**
    * Return the current scanline
    * WARNING: if sync = true, SPI must NOT be in use !
    **/
    int _getScanLine(bool sync);


    /**
    * estimate the current refresh rate by banging the get_scanline 0x45 command
    **/
    void _sampleRefreshRate();


    /**
    * estimate the refreshrate (in Hz) for a given mode based on the refresh rate for mode 0
    **/
    float _refreshRateForMode(int mode) const;


    /**
    * Find the mode with closest refresh rate.
    **/
    int _modeForRefreshRate(float hz) const;





    /**********************************************************************************************************
    * IntervalTimer
    ***********************************************************************************************************/


    IntervalTimer _it;      // the PIT timer object 

    static ILI9341Driver* volatile _pitObj[4];   // point back to this->
    volatile int _pitindex; // index for which pitObj this-> refers to. 
    volatile bool _istimer; // true if a timer is currently waiting to ring. 

    static void _pitcb0() { if (_pitObj[0]) { noInterrupts(); _pitObj[0]->_it.end(); _pitObj[0]->_istimer = false; interrupts(); ((_pitObj[0])->*(_pitObj[0]->_pitcb))(); } }  // forward to the timer method cb
    static void _pitcb1() { if (_pitObj[1]) { noInterrupts(); _pitObj[1]->_it.end(); _pitObj[1]->_istimer = false; interrupts(); ((_pitObj[1])->*(_pitObj[1]->_pitcb))(); } }  // forward to the timer method cb
    static void _pitcb2() { if (_pitObj[2]) { noInterrupts(); _pitObj[2]->_it.end(); _pitObj[2]->_istimer = false; interrupts(); ((_pitObj[2])->*(_pitObj[2]->_pitcb))(); } }  // forward to the timer method cb
    static void _pitcb3() { if (_pitObj[3]) { noInterrupts(); _pitObj[3]->_it.end(); _pitObj[3]->_istimer = false; interrupts(); ((_pitObj[3])->*(_pitObj[3]->_pitcb))(); } }  // forward to the timer method cb

    volatile methodCB_t _pitcb;        // timer callback method. 


    /** call at startup to initialize the timer */
    void _timerinit();


    /** Set the timer to ring in us microseconds. */
    void _setTimerIn(uint32_t us, methodCB_t timercb) ILI9341_T4_ALWAYS_INLINE
        {
        noInterrupts();
        _it.end(); // stop ongoing timer before changing callback method. 
        _pitcb = timercb;
        if ((us <= 3) || (us > ILI9341_T4_MAX_DELAY_MICROSECONDS)) { us = 3; } // asap
        _it.priority(ILI9341_T4_IRQ_PRIORITY);
        _istimer = true;
        switch (_pitindex)
            {
            case 0: 
                {
                _it.begin(_pitcb0, us); 
                break;
                }
            case 1: 
                {
                _it.begin(_pitcb1, us); 
                break;
                }
            case 2: 
                {
                _it.begin(_pitcb2, us); 
                break;
                }
            case 3: 
                {
                _it.begin(_pitcb3, us); 
                break;
                }
            }
        interrupts();
        }


    /** Set the timer to ring when micros() reaches ustime */
    void _setTimerAt(uint32_t ustime, methodCB_t timercb) ILI9341_T4_ALWAYS_INLINE
        {
        const uint32_t m = micros();
        const uint32_t max_us = (max(_vsync_spacing, 1) + 1) * _period;
        uint32_t us;
        if (ustime <= m)
            {
            us = 1; 
            }
        else if (ustime > m + max_us)
            {
            us = max_us;
            _printf("Abnormally large value for setTimerAt() : %d us", ustime - m);
            }
        else
            {
            us = ustime - m;
            }
        _setTimerIn(us, timercb);
        }


    /** Cancel the timer (if ticking). */
    void _cancelTimer() ILI9341_T4_ALWAYS_INLINE
    {
        noInterrupts();
        _it.end();
        _istimer = false;
        interrupts();
    }


    /** Query if the timer is currently ticking. */
    bool _isTimer() const ILI9341_T4_ALWAYS_INLINE
    {
        return _istimer;
    }



    /**********************************************************************************************************
    * Drawing characters
    * (adapted from the tgx library)
    ***********************************************************************************************************/

    static uint16_t _blend32(uint32_t bg_col, uint32_t fg_col, uint32_t a) ILI9341_T4_ALWAYS_INLINE
        {
        const uint32_t bg = (bg_col | (bg_col << 16)) & 0b00000111111000001111100000011111;
        const uint32_t fg = (fg_col | (fg_col << 16)) & 0b00000111111000001111100000011111;
        const uint32_t result = ((((fg - bg) * a) >> 5) + bg) & 0b00000111111000001111100000011111;
        return (uint16_t)((result >> 16) | result); // contract result
        }


    static uint32_t _fetchbit(const uint8_t* p, uint32_t index) ILI9341_T4_ALWAYS_INLINE  { return (p[index >> 3] & (0x80 >> (index & 7))); }


    static uint32_t _fetchbits_unsigned(const uint8_t* p, uint32_t index, uint32_t required);


    static uint32_t _fetchbits_signed(const uint8_t* p, uint32_t index, uint32_t required);


    static bool _clipit(int& x, int& y, int& sx, int& sy, int& b_left, int& b_up, int lx, int ly);


    static void _measureChar(char c, int pos_x, int pos_y, int & min_x, int & max_x, int & min_y, int & max_y, const void* pfont, int & xadvance);


    static void _measureText(const char* text, int pos_x, int pos_y, int& min_x, int& max_x, int& min_y, int& max_y, const void* pfont, bool start_newline_at_0);


    static void _drawTextILI(const char* text, int pos_x, int pos_y, uint16_t col, const void* pfont, bool start_newline_at_0, int lx, int ly, int stride, uint16_t* buffer, float opacity);


    static void _drawCharILI(char c, int & pos_x, int & pos_y, uint16_t col, const void * pfont, int lx, int ly, int stride, uint16_t* buffer, float opacity);


    static void _drawCharBitmap_4BPP(const uint8_t* bitmap, int rsx, int b_up, int b_left, int sx, int sy, int x, int y, uint16_t col, int stride, uint16_t* buffer, float opacity);

    static void _fillRect(int xmin, int xmax, int ymin, int ymax, int lx, int ly, int stride, uint16_t* buffer, uint16_t color, float opacity);

    void _uploadText(const char* text, int pos_x, int pos_y, uint16_t col, uint16_t col_bg, const void* pfont, bool start_newline_at_0);

    void _uploadChar(char c, int & pos_x, int & pos_y, uint16_t col, uint16_t col_bg, const void* pfont);


  



    /**********************************************************************************************************
    * About Stats
    ***********************************************************************************************************/


    uint32_t        _stats_nb_frame;            // number of frame drawn since last reset. 
    elapsedMillis   _stats_elapsed_total;       // total time since the last reset. 

    elapsedMicros   _stats_elapsed_cputime;     // timer for the cpu time use during a frame
    uint32_t        _stats_cputime;             // cpu time spend in a frame
    StatsVar        _statsvar_cputime;          // statistics about the cpu time usage. 

    elapsedMicros   _stats_elapsed_uploadtime;  // timer for the dma time single frame
    uint32_t        _stats_uploadtime;          // cpu time spend in a frame
    StatsVar        _statsvar_uploadtime;       // statistics about the cpu time usage. 

    uint32_t        _stats_nb_uploaded_pixels;  // number of pixel upload during a frame. 
    StatsVar        _statsvar_uploaded_pixels;  // statistics about the number of pixels uploaded per frame.

    uint32_t        _stats_nb_transactions;     // number of transactions for the frame.
    StatsVar        _statsvar_transactions;     // statistics about the number of transactions per frame.

    StatsVar        _statsvar_margin;           // statistics about the 'margin' per frame

    StatsVar        _statsvar_vsyncspacing;     // statistics about the effective vsync_spacing

    uint32_t        _nbteared;                  // number of frame for which screen tearing may have occured. 


    uint32_t        _stats_current_fps;         // current fps

    StatsVar        _statsvar_fps;              // statistics about the framerate

    elapsedMillis   _fps_counter;               // counter for the instant fps. 

    uint32_t        _fps_ticks;                 // number of ticks. 


    void _startframe(bool vsynonc)
    {
        _stats_nb_uploaded_pixels = 0;
        _stats_nb_transactions = 0;
        _stats_cputime = 0;;
        _stats_elapsed_cputime = 0;
        _stats_uploadtime = 0;
        _stats_elapsed_uploadtime = 0;
    }


    void _restartCpuTime() ILI9341_T4_ALWAYS_INLINE
    {
        _stats_elapsed_cputime = 0;
    }


    void _pauseCpuTime() ILI9341_T4_ALWAYS_INLINE
    {
        _stats_cputime += _stats_elapsed_cputime;
    }


    void _restartUploadTime() ILI9341_T4_ALWAYS_INLINE
    {
        _stats_elapsed_uploadtime = 0;
    }


    void _pauseUploadTime() ILI9341_T4_ALWAYS_INLINE
    {
        _stats_uploadtime += _stats_elapsed_uploadtime;
    }


    void _endframe();






    /**********************************************************************************************************
    * About SPI
    ***********************************************************************************************************/


    uint32_t _spi_clock;                        // spi write speed
    uint32_t _spi_clock_read;                   // spi read speed

    uint8_t _cs, _dc;                           // hardware pins
    uint8_t _sclk, _mosi, _miso;                // for the screen
    uint8_t _rst;                               // 

    uint8_t _touch_cs, _touch_irq;              // pins for XPT2046 if present

    uint8_t  _spi_num;                          //  spi busd number: 0 1 or 2
    SPIClass* _pspi = nullptr;                  // the main spi object
    IMXRT_LPSPI_t* _pimxrt_spi;                 // Raw access to the spi registers (same as _pspi->port() private method). 
    SPIClass::SPI_Hardware_t* _spi_hardware;    // Raw internal spi hardware (same as _pspi->hardware() private method). Hacked from _pspi and used for DMA transfer.

    uint32_t _cspinmask;                        // mask for the CS pin when writing directly to register port (used by directWriteHigh/Low methods)
    volatile uint32_t* _csport;                 // port to which the CS pin belongs (used by directWriteHigh/Low methods)

    bool _hardware_dc;                          // true if DC is on a valid CS pin so we can use hardware speedup
    uint32_t _dcpinmask;                        // mask for the CS pin when writing directly to register port (used by directWriteHigh/Low methods)
    volatile uint32_t* _dcport;                 // port to which the CS pin belongs (used by directWriteHigh/Low methods)

    uint32_t _spi_tcr_current;                  // current value of the transfer command register (TCR) (see chap 48 of the IMXRT1060 manual). 

    uint32_t _tcr_dc_assert;                    // mask for the TCR register when DC is asserted (low)
    uint32_t _tcr_dc_not_assert;                // mask for the TCR register when DC is not asserted (high)

    volatile uint32_t  _dma_spi_tcr_deassert;   // TCR value for deasserting DC
    volatile uint32_t  _dma_spi_tcr_assert;     // TCR value for asserting DC


    uint8_t _pending_rx_count;                  // hack ...


    /** for debugging PASET and CASET errors*/
    void _drawRect(int xmin, int xmax, int ymin, int ymax, uint16_t color);


    void _beginSPITransaction(uint32_t clock) ILI9341_T4_ALWAYS_INLINE
        {
        _pspi->beginTransaction(SPISettings(clock, MSBFIRST, SPI_MODE0));
        _spi_tcr_current = _pimxrt_spi->TCR; //  DC is on hardware CS
        _pending_rx_count = 0;
        if (_csport) _directWriteLow(_csport, _cspinmask); // drive CS low
        }


    void _endSPITransaction() ILI9341_T4_ALWAYS_INLINE
        {
        if (_csport) _directWriteHigh(_csport, _cspinmask); // drive CS high
        _pspi->endTransaction();
        }


    uint8_t _readcommand8(uint8_t reg, uint8_t index = 0, int timeout_ms = 10);


    void _writecommand_cont(uint8_t c) ILI9341_T4_ALWAYS_INLINE
        {
        _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_CONT); // 
        _pimxrt_spi->TDR = c;
        _pending_rx_count++; //
        _waitFifoNotFull();
        }


    void _writedata8_cont(uint8_t c) ILI9341_T4_ALWAYS_INLINE
        {
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_CONT);
        _pimxrt_spi->TDR = c;
        _pending_rx_count++; //
        _waitFifoNotFull();
        }


    void _writedata16_cont(uint16_t d) ILI9341_T4_ALWAYS_INLINE
        {
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(15) | LPSPI_TCR_CONT);
        _pimxrt_spi->TDR = d;
        _pending_rx_count++; //
        _waitFifoNotFull();
        }


    void _writecommand_last(uint8_t c) ILI9341_T4_ALWAYS_INLINE
        {   
        _maybeUpdateTCR(_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7));
        _pimxrt_spi->TDR = c;
        // _pimxrt_spi->SR = LPSPI_SR_WCF | LPSPI_SR_FCF | LPSPI_SR_TCF;
        _pending_rx_count++; //
        _waitTransmitComplete();
        }


    void _writedata8_last(uint8_t c) ILI9341_T4_ALWAYS_INLINE
        {
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7));
        _pimxrt_spi->TDR = c;
        // _pimxrt_spi->SR = LPSPI_SR_WCF | LPSPI_SR_FCF | LPSPI_SR_TCF;
        _pending_rx_count++; //
        _waitTransmitComplete();
        }


    void _writedata16_last(uint16_t d) ILI9341_T4_ALWAYS_INLINE
        {
        _maybeUpdateTCR(_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(15));
        _pimxrt_spi->TDR = d;
        // _pimxrt_spi->SR = LPSPI_SR_WCF | LPSPI_SR_FCF | LPSPI_SR_TCF;
        _pending_rx_count++; //
        _waitTransmitComplete();
        }



#define ILI9341_T4_TCR_MASK  (LPSPI_TCR_PCS(3) | LPSPI_TCR_FRAMESZ(31) | LPSPI_TCR_CONT | LPSPI_TCR_RXMSK)
#define ILI9341_T4_DC_PCS_MASK   (LPSPI_TCR_PCS(3))


    void _maybeUpdateTCR(uint32_t requested_tcr_state) ILI9341_T4_ALWAYS_INLINE
        {
        const uint32_t spi_tcr_new = (_spi_tcr_current & (~ILI9341_T4_TCR_MASK)) | (requested_tcr_state & ILI9341_T4_TCR_MASK);        
        if (spi_tcr_new != _spi_tcr_current)
            {
            if (_hardware_dc)
                { // DC is controlled by SPI
//                while ((_pimxrt_spi->FSR & 0x1f)); // wait for output fifo to be empty.
                _pimxrt_spi->TCR = spi_tcr_new; // update the TCR    
                }
            else
                { // DC is controlled by software.                                     
                // update the TCR anyway (dirty trick to also adds an instruction in the fifo anyway :))
                _pimxrt_spi->TCR = (spi_tcr_new & (~ILI9341_T4_DC_PCS_MASK)) | (_tcr_dc_not_assert & ILI9341_T4_DC_PCS_MASK);
                if ( (spi_tcr_new & ILI9341_T4_DC_PCS_MASK) != (_spi_tcr_current & ILI9341_T4_DC_PCS_MASK) )
                    { // we must toogle DC
                    while ((_pimxrt_spi->FSR & 0x1f)); // wait for output fifo to be empty (since the last instruction is a TCR command, it insures that we have good timing even if the last instruction is not yet executed !)                                                        
                    if ( (spi_tcr_new & ILI9341_T4_DC_PCS_MASK) == (_tcr_dc_not_assert & ILI9341_T4_DC_PCS_MASK) )
                        _directWriteHigh(_dcport, _dcpinmask);
                    else
                        _directWriteLow(_dcport, _dcpinmask);
                    }
                }                
            _spi_tcr_current = spi_tcr_new;
            }
        }    


    
    void _dma_assert_dc() ILI9341_T4_ALWAYS_INLINE
        {       
        _pimxrt_spi->TCR = _dma_spi_tcr_assert;
        if (!_hardware_dc)
            { // DC is controlled by SPI
            while ((_pimxrt_spi->FSR & 0x1f)); // wait for output fifo to be empty (since the last instruction is a TCR command, it insures that we have good timing even if the last instruction is not yet executed !)                                                        
            _directWriteLow(_dcport, _dcpinmask);
            }
        }
    
    void _dma_deassert_dc() ILI9341_T4_ALWAYS_INLINE
        {
        _pimxrt_spi->TCR = _dma_spi_tcr_deassert;
        if (!_hardware_dc)
            { // DC is controlled by SPI
            while ((_pimxrt_spi->FSR & 0x1f)); // wait for output fifo to be empty (since the last instruction is a TCR command, it insures that we have good timing even if the last instruction is not yet executed !)                                                        
            _directWriteHigh(_dcport, _dcpinmask);
            }
        }


    void _dma_set_base_tcr()
        {
        if (_hardware_dc)
            {
            _dma_spi_tcr_assert = (_spi_tcr_current & ~ILI9341_T4_TCR_MASK) | (_tcr_dc_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_RXMSK);
            _dma_spi_tcr_deassert = (_spi_tcr_current & ~ILI9341_T4_TCR_MASK) | (_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(15) | LPSPI_TCR_RXMSK); // bug with | LPSPI_TCR_CONT
            }
        else
            {
            _dma_spi_tcr_assert = (_spi_tcr_current & ~ILI9341_T4_TCR_MASK) | (_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_RXMSK);
            _dma_spi_tcr_deassert = (_spi_tcr_current & ~ILI9341_T4_TCR_MASK) | (_tcr_dc_not_assert | LPSPI_TCR_FRAMESZ(15) | LPSPI_TCR_RXMSK); // bug with | LPSPI_TCR_CONT
            }
        }


    void _waitFifoNotFull();


    void _waitTransmitComplete();


    //. From Onewire utility files
    void _directWriteLow(volatile uint32_t* base, uint32_t mask) ILI9341_T4_ALWAYS_INLINE { *(base + 34) = mask; }

    void _directWriteHigh(volatile uint32_t* base, uint32_t mask) ILI9341_T4_ALWAYS_INLINE { *(base + 33) = mask; }

    void _toggle(volatile uint32_t* base, uint32_t mask) ILI9341_T4_ALWAYS_INLINE { *(base + 35) = mask; }


    /**********************************************************************************************************
    * About Touch
    ***********************************************************************************************************/


    volatile int _touch_request_read;           // flag set to true when reading is requested. 

    elapsedMillis _em_touched_read;             // number of ms since the touch position was last read. 
    elapsedMillis _em_touched_irq;              // number of ms since the last touch irq occured
    volatile bool _touched;                     // true if touch irq has occured 
    volatile bool _touched_read;                // true if touch irq has occured 
    volatile int _touch_x, _touch_y, _touch_z;  // last touch position 

    volatile int _touch_z_threshold;            // threshold for touch detection
    volatile bool _touch_has_calibration;       // true if touch calibration is enabled
    volatile int _touch_calib[4];               // touch calibration value

    static ILI9341Driver* volatile _touchObjects[4];   // point back to this->

    static void _touch_int0() { if (_touchObjects[0]) { _touchObjects[0]->_touch_int(); } }  // forward to the touch interrupt method cb
    static void _touch_int1() { if (_touchObjects[1]) { _touchObjects[1]->_touch_int(); } }  // forward to the touch interrupt method cb
    static void _touch_int2() { if (_touchObjects[2]) { _touchObjects[2]->_touch_int(); } }  // forward to the touch interrupt method cb
    static void _touch_int3() { if (_touchObjects[3]) { _touchObjects[3]->_touch_int(); } }  // forward to the touch interrupt method cb

    /** the touch interrupt */
    void _touch_int()
        {
        _touched = true;
        _touched_read = true;
        _em_touched_irq = 0;
        }


    /** set the touch interrupt routine */
    void _setTouchInterrupt();


    /** update the touch position via spi read (if needed) */
    void _updateTouch();

    /** update the touch position via spi read (if needed), may be called at dma completion */
    void _updateTouch2();

    /** poor man's noise filtering */
    static int16_t _besttwoavg(int16_t x, int16_t y, int16_t z);


    /** convert from raw value to x coord (in orientation 0) */
    int _mapTouchX(int x, int A, int B)
        {
        return ILI9341Driver::_clip<int>((int)roundf(ILI9341_T4_TFTWIDTH * ((float)(x - A)) / (B - A)), (int)0, (int)ILI9341_T4_TFTWIDTH - 1);
        }

    /** convert from raw value to y coord (in orientation 0) */
    int _mapTouchY(int y, int C, int D)
        {
        return ILI9341Driver::_clip<int>((int)roundf(ILI9341_T4_TFTHEIGHT * ((float)(y - C)) / (D - C)), (int)0, (int)ILI9341_T4_TFTHEIGHT - 1);
        }


    /** draw a calibration rectangle */
    void _calibRect(int cx, int cy, int R, uint16_t col_ext, uint16_t col_int);


    /** used for touch calibration */
    void _calibTouch(int& x, int& y, int& z, int prv_x = -1, int prv_y = -1);



};



}

#endif 

#endif
/** end of file */


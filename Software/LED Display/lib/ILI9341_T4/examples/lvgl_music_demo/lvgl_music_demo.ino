/********************************************************************
*
* ILI9341_T4 library example. Interfacing with the LVGL library
*
* 
* music player demo from lvgl adapted to Teensy 4 with ILI9341_T4.
* 
* 
* This example demonstrates how to use the ILI9341_T4 driver with the 
* LVGL library. Here, allocate an internal framebuffer of size 320x240 
* for the ILI9341_T4 driver but only a small 320x40 buffer for LVGL 
* to draw onto. Then we use the updateRegion() method from the library 
* to update the internal framebuffer and sync with the screen using
* differential updates for maximum performance. 
*
* The total memory consumption for the 'graphic part' is 191KB:
*
*   - 150KB for the internal framebuffer 
*   - 25KB for LVGL draw buffer
*   - 16KB for 2 diffs buffer with 8Kb each. 
*    
* -----------------------------------
* BUILDING THE EXAMPLE (FOR ARDUINO)
* -----------------------------------
* 
* (1) Install the 'lvgl' libraries in Arduino's library folder. 
*     You can install it directly from Arduino's IDE or simply copy 
*     the github repo: https://github.com/lvgl/lvgl/ to Arduino's
*     librarie folder (tested here with LVGL v8.2). 
*
* (2) Copy and rename the file 'libraries/lvgl/lv_conf_template.h' to 
*     'libraries/lv_conf.h' (i.e. put this file directly in Arduino's 
*     libraries root folder). 
*     
* (3) Edit the file 'lv_conf.h' such that:
* 
*     -> Replace '#if 0' by '#if 1'               (at the begining of the file)
*     -> set #define LV_COLOR_DEPTH 16            (should be already set to the correct value)
*     -> set #define LV_DISP_DEF_REFR_PERIOD 15   (change this from 30ms to 15ms for more reactive display)
*     -> set #define LV_USE_PERF_MONITOR 1        (if you want to to show the FPS counter)
*     -> set #define LV_FONT_MONTSERRAT_12  1     (should be already set to the correct value)
*     -> set #define LV_FONT_MONTSERRAT_14  1
*     -> set #define LV_FONT_MONTSERRAT_16  1
*
********************************************************************/


#include <ILI9341_T4.h>

#include <lvgl.h> // see the comment above for infos about installing and configuring LVGL. 

#include "src/music/lv_demo_music.h" // the src/music/ folder contain the source code for LVGL music demo. 

//
// DEFAULT WIRING USING SPI 0 ON TEENSY 4/4.1
//
// 
#define PIN_SCK     13      // mandatory
#define PIN_MISO    12      // mandatory
#define PIN_MOSI    11      // mandatory
#define PIN_DC      10      // mandatory, can be any pin but using pin 10 (or 36 or 37 on T4.1) provides greater performance

#define PIN_CS      9       // optional (but recommended), can be any pin.  
#define PIN_RESET   6       // optional (but recommended), can be any pin. 
#define PIN_BACKLIGHT 255   // optional, set this only if the screen LED pin is connected directly to the Teensy.
#define PIN_TOUCH_IRQ 255   // optional, set this only if the touchscreen is connected on the same SPI bus
#define PIN_TOUCH_CS  255   // optional, set this only if the touchscreen is connected on the same spi bus


//
// ALTERNATE WIRING USING SPI 1 ON TEENSY 4/4.1 
//
//#define PIN_SCK     27      // mandatory 
//#define PIN_MISO    1       // mandatory
//#define PIN_MOSI    26      // mandatory
//#define PIN_DC      0       // mandatory, can be any pin but using pin 0 (or 38 on T4.1) provides greater performance

//#define PIN_CS      30      // optional (but recommended), can be any pin.  
//#define PIN_RESET   29      // optional (but recommended), can be any pin.  
//#define PIN_BACKLIGHT 255   // optional, set this only if the screen LED pin is connected directly to the Teensy. 
//#define PIN_TOUCH_IRQ 255   // optional, set this only if the touchscreen is connected on the same SPI bus
//#define PIN_TOUCH_CS  255   // optional, set this only if the touchscreen is connected on the same spi bus



// 40MHz SPI. Can do much better with short wires
#define SPI_SPEED       40000000

// screen size in landscape mode
#define LX  320
#define LY  240

// 2 diff buffers with about 8K memory each
ILI9341_T4::DiffBuffStatic<8000> diff1;
ILI9341_T4::DiffBuffStatic<8000> diff2;

// the internal framebuffer for the ILI9341_T4 driver (150KB) 
// in DMAMEM to save space in the lower (faster) part of RAM. 
DMAMEM uint16_t internal_fb[LX * LY];

// the screen driver object
ILI9341_T4::ILI9341Driver tft(PIN_CS, PIN_DC, PIN_SCK, PIN_MOSI, PIN_MISO, PIN_RESET, PIN_TOUCH_CS, PIN_TOUCH_IRQ);


// number of lines in lvgl's internal draw buffer 
#define BUF_LY 40

lv_color_t lvgl_buf[LX*BUF_LY]; // memory for lvgl draw buffer (25KB) 

lv_disp_draw_buf_t draw_buf;    // lvgl 'draw buffer' object
lv_disp_drv_t disp_drv;         // lvgl 'display driver'
lv_indev_drv_t indev_drv;       // lvgl 'input device driver'



/** Callback to draw on the screen */
void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
    {
    const bool redraw_now = lv_disp_flush_is_last(disp);  // check if when should update the screen (or just buffer the changes). 
    tft.updateRegion(redraw_now, (uint16_t*)color_p, area->x1, area->x2, area->y1, area->y2); // update the interval framebuffer and then redraw the screen if requested
    lv_disp_flush_ready(disp); // tell lvgl that we are done and that the lvgl draw buffer can be reused.  
    }


 
IntervalTimer guiTimer;

// callback to update lvgl's tick every ms. 
void guiInc() 
    {
    lv_tick_inc(1);
    }



void setup() 
    {
    Serial.begin(9600);

    // ------------------------------
    // Init the ILI9341_T4 driver. 
    // ------------------------------
    tft.output(&Serial);                // send debug info to serial port.
    while (!tft.begin(SPI_SPEED));      // init
    tft.setFramebuffer(internal_fb);    // set the internal framebuffer
    tft.setDiffBuffers(&diff1, &diff2); // set the diff buffers
    tft.setRotation(1);                 // landscape mode 1 : 320x240
    tft.setDiffGap(4);                  // with have large 8K diff buffers so we can use a small gap. 
    tft.setVSyncSpacing(1);             // lvgl is already controlling framerate: we just set this to 1 to minimize screen tearing. 
    tft.setRefreshRate(100);            // 100Hz refresh, why not...
    tft.clear(0);                       // black screen to start. 


    // ------------------------------
    // Init LVGL
    // ------------------------------
    lv_init();

    // initialize lvgl drawing buffer
    lv_disp_draw_buf_init(&draw_buf, lvgl_buf, nullptr, LX * BUF_LY);

    // Initialize lvgl display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LX;
    disp_drv.ver_res = LY;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // set the interval timer that given lvgl ticks. 
    guiTimer.begin(guiInc, 1000);


    // ------------------------------
    // Run the demo
    // ------------------------------
    lv_demo_music();
    }




elapsedMillis em = 0; 

void loop() 
    {
    lv_task_handler(); // lvgl gui handler

    delay(5); // plenty of time left to do other stuff... 
    
    // print driver infos every 5 seconds 
    if (em > 5000)
        {
        em = 0;
        // NOTE: the framerate stats are wrong because lvgl uses partial 
        // redraw with tft.updateRegion() and also does not redraw
        // the screen when nothing changes. Thus the actual framerate 
        // (displayed by lvgl) is the correct one (and much better :-))
        // The other statistics are corect.
        tft.printStats();
        }
    }


/** end of file */


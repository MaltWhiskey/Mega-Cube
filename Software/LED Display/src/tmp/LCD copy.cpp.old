/********************************************************************
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
 * Edit the file 'lv_conf.h' such that:
 *     -> Replace '#if 0' by '#if 1'
 *     -> set #define LV_COLOR_DEPTH 16
 *     -> set #define LV_DISP_DEF_REFR_PERIOD 15
 *     -> set #define LV_USE_PERF_MONITOR 0
 *     -> set #define LV_FONT_MONTSERRAT_12  1
 *     -> set #define LV_FONT_MONTSERRAT_14  1
 *     -> set #define LV_FONT_MONTSERRAT_16  1
 ********************************************************************/
#include <ILI9341_T4.h>
#include <gui/ui.h>
#include <lvgl.h>

#define PIN_SCK 13
#define PIN_MISO 12
#define PIN_MOSI 11
#define PIN_DC 20  // Using pin 10 provides greater performance
#define PIN_CS 15
#define PIN_RESET 21
#define PIN_BACKLIGHT 22
#define PIN_TOUCH_CS 18
#define PIN_TOUCH_IRQ 19

#define SPI_SPEED 30000000
#define LX 320
#define LY 240
#define BUF_LY 40

// 2 diff buffers with about 8K memory each
DMAMEM ILI9341_T4::DiffBuffStatic<8000> diff1;
DMAMEM ILI9341_T4::DiffBuffStatic<8000> diff2;
// the internal framebuffer for the ILI9341_T4 driver (150KB)
// in DMAMEM to save space in the lower (faster) part of RAM.
DMAMEM uint16_t internal_fb[LX * LY];
// number of lines in lvgl's internal draw buffer (25K)
DMAMEM lv_color_t lvgl_buf[LX * BUF_LY];

// the screen driver object
ILI9341_T4::ILI9341Driver tft(PIN_CS, PIN_DC, PIN_SCK, PIN_MOSI, PIN_MISO,
                              PIN_RESET, PIN_TOUCH_CS, PIN_TOUCH_IRQ);

lv_disp_draw_buf_t draw_buf;  // lvgl 'draw buffer' object
lv_disp_drv_t disp_drv;       // lvgl 'display driver'
lv_indev_drv_t indev_drv;     // lvgl 'input device driver'

// Callback to draw on the screen
void cb_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area,
                   lv_color_t* color_p) {
  // check if when should update the screen (or just buffer the changes).
  const bool redraw_now = lv_disp_flush_is_last(disp);
  // update the interval framebuffer and then redraw the screen if requested
  tft.updateRegion(redraw_now, (uint16_t*)color_p, area->x1, area->x2, area->y1,
                   area->y2);
  // tell lvgl that we are done and that the lvgl draw buffer can be reused
  lv_disp_flush_ready(disp);
}

// Callback to read the touchpad
void cb_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data) {
  int touchX, touchY, touchZ;
  bool touched = tft.readTouch(touchX, touchY, touchZ);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  }
}

// Callback to update lvgl's tick every ms.
void cb_tick_inc() { lv_tick_inc(1); }

void setup_lcd() {
  // send debug info to serial port.
  tft.output(&Serial);
  while (!tft.begin(SPI_SPEED))
    ;
  tft.setFramebuffer(internal_fb);
  tft.setDiffBuffers(&diff1, &diff2);
  tft.setRotation(1);
  // with have large 8K diff buffers so we can use a small gap.
  tft.setDiffGap(4);
  // lvgl is already controlling framerate: we just set
  // this to 1 to minimize screen tearing.
  tft.setVSyncSpacing(1);
  tft.setRefreshRate(180);

  if (PIN_BACKLIGHT != 255) {
    pinMode(PIN_BACKLIGHT, OUTPUT);
    digitalWrite(PIN_BACKLIGHT, HIGH);
  }

  // touchscreen XPT2046 on the same SPI bus
  // tft.calibrateTouch();
  int touch_calib[4] = {3820, 335, 3890, 436};
  tft.setTouchCalibration(touch_calib);

  // Init LVGL
  lv_init();
  // initialize lvgl drawing buffer
  lv_disp_draw_buf_init(&draw_buf, lvgl_buf, nullptr, LX * BUF_LY);
  // Initialize lvgl display driver
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LX;
  disp_drv.ver_res = LY;
  disp_drv.flush_cb = cb_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Initialize lvgl input device driver (the touch screen)
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = cb_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // Set the interval timer giving lvgl ticks.
  static IntervalTimer guiTimer;
  guiTimer.begin(cb_tick_inc, 1000);

  // Generate the GUI (generate with Squareline Studio)
  ui_init();
}

// lvgl gui handler
void loop_lcd() { lv_task_handler(); }

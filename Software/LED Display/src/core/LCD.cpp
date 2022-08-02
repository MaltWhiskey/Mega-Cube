#include "LCD.h"

ILI9341_T4::DiffBuffStatic<8000> LCD::diff1;
ILI9341_T4::DiffBuffStatic<8000> LCD::diff2;
uint16_t LCD::internal_fb[LX * LY];
lv_color_t LCD::lvgl_buf[LX * BUF_LY];
ILI9341_T4::ILI9341Driver LCD::tft(TFT_CS, TFT_DC, SCK, MOSI, MISO, TFT_RESET,
                                   TOUCH_CS, TOUCH_IRQ);
lv_disp_draw_buf_t LCD::draw_buf;
lv_disp_drv_t LCD::disp_drv;
lv_indev_drv_t LCD::indev_drv;

void LCD::begin() {
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

  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // touchscreen XPT2046 on the same SPI bus
  // tft.calibrateTouch();
  int touch_calib[4] = {TS_MAXX, TS_MINX, TS_MAXY, TS_MINY};
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
  // static IntervalTimer guiTimer;
  // guiTimer.begin(cb_tick_inc, 1000);

  // Generate the GUI (generate with Squareline Studio)
  ui_init();
}

// lvgl gui handler
void LCD::loop() { lv_task_handler(); }

// Callback to draw on the screen
void LCD::cb_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area,
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
void LCD::cb_touchpad_read(lv_indev_drv_t* indev_driver,
                           lv_indev_data_t* data) {
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
// void LCD::cb_tick_inc() { lv_tick_inc(1); }

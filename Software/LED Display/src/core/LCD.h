#ifndef LCD_H
#define LCD_H
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

class LCD {
 public:
  static const uint8_t TFT_LED = 22;
  static const uint8_t TFT_RESET = 21;
  // Using pin 10 for TFT_DC provides greater performance
  static const uint8_t TFT_DC = 20;
  static const uint8_t TOUCH_IRQ = 19;
  static const uint8_t TOUCH_CS = 18;
  static const uint8_t SD_CS = 17;
  static const uint8_t TFT_CS = 15;
  static const uint8_t SCK = 13;
  static const uint8_t MISO = 12;
  static const uint8_t MOSI = 11;

  static const uint16_t TS_MAXX = 3900;
  static const uint16_t TS_MINX = 340;
  static const uint16_t TS_MAXY = 3900;
  static const uint16_t TS_MINY = 200;

  static const uint16_t LX = 320;
  static const uint16_t LY = 240;

 private:
  static const uint16_t BUF_LY = 40;
  static const uint32_t SPI_SPEED = 30000000;

 private:
  // 2 diff buffers with about 8K memory each
  DMAMEM static ILI9341_T4::DiffBuffStatic<8000> diff1;
  DMAMEM static ILI9341_T4::DiffBuffStatic<8000> diff2;
  // the internal framebuffer for the ILI9341_T4 driver (150KB)
  // in DMAMEM to save space in the lower (faster) part of RAM.
  DMAMEM static uint16_t internal_fb[LX * LY];
  // number of lines in lvgl's internal draw buffer (25K)
  DMAMEM static lv_color_t lvgl_buf[LX * BUF_LY];

  // the screen driver object
  static ILI9341_T4::ILI9341Driver tft;
  // lvgl 'draw buffer' object
  static lv_disp_draw_buf_t draw_buf;
  // lvgl 'display driver'
  static lv_disp_drv_t disp_drv;
  // lvgl 'input device driver'
  static lv_indev_drv_t indev_drv;

 public:
  static void begin();
  static void loop();
  static void cb_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area,
                            lv_color_t* color_p);
  static void cb_touchpad_read(lv_indev_drv_t* indev_driver,
                               lv_indev_data_t* data);
  static void cb_tick_inc();
};
#endif
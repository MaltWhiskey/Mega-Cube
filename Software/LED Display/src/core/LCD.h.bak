#ifndef LCD_H
#define LCD_H
#include <Arduino.h>
#include <ILI9341_t3n.h>
#include <SD.h>
#include <XPT2046_Touchscreen.h>
#include <stdint.h>

class LCD {
 public:
  static const uint8_t TFT_LED = 22;
  static const uint8_t TFT_RESET = 21;
  static const uint8_t TFT_DC = 20;
  static const uint8_t TOUCH_IRQ = 19;
  static const uint8_t TOUCH_CS = 18;
  static const uint8_t SD_CS = 17;
  static const uint8_t TFT_CS = 15;
  static const uint8_t SCK = 13;
  static const uint8_t MISO = 12;
  static const uint8_t MOSI = 11;

  static const uint16_t TS_MINX = 340;
  static const uint16_t TS_MAXX = 3900;
  static const uint16_t TS_MINY = 200;
  static const uint16_t TS_MAXY = 3900;

  static XPT2046_Touchscreen ts;
  static ILI9341_t3n tft;
  static SDClass sd;

  uint8_t *picture;

  void begin();
  void update();
  void print(String s);
  void bmpDraw(const char *filename, uint8_t x, uint16_t y);
  uint16_t read16(File &f);
  uint32_t read32(File &f);
};
#endif
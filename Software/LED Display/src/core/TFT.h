#ifndef TFT_H
#define TFT_H
#include <Arduino.h>
#include <stdint.h>

class TFT {
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

 public:
  // Gets a reference to enable calling public methods
  static TFT &instance() {
    // Create exactly one Display object with the private contructor
    static TFT tft;
    // Return the one and only display object
    return tft;
  };
  // Not allowed to make an object of the class (singleton pattern)
  TFT(){};
  // Copy constructor is also disabled (singleton pattern)
  TFT(TFT const &);
  // Assigment operator is also disabled (singleton pattern)
  void operator=(TFT const &);
  // Do not use a class contructor to start the display (Arduino compatibility)
  void begin();
  void loop();
};
#endif
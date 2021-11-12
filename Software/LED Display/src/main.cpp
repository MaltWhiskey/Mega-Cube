
#include <Arduino.h>

#include "Core/Config.h"
#include "Core/tft.h"
#include "Space/animation.h"
/*---------------------------------------------------------------------------------------
 * Globals
 *-------------------------------------------------------------------------------------*/
Config config;
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  Animation::begin();
  Serial.begin(115200);
  delay(5000);
  TFT::instance().begin();
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  // Print FPS once every second
  static Timer print_interval = 1.0f;
  Animation::animate();
  if (print_interval.update()) {
    Serial.printf("FPS=%1.2f\n", Animation::fps());
  }
  // TFT::instance().loop();
}
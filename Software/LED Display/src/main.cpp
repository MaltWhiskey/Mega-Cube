
#include <Arduino.h>

#include "core/tft.h"
#include "space/animation.h"
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
Timer t = 1.0f;
void loop() {
  Animation::animate();
  // Print FPS once every second
  if (t.update()) {
    Serial.printf("FPS=%1.2f\n", Animation::fps());
  }
  // TFT::instance().loop();
}
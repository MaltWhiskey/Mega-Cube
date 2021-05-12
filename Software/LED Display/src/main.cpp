
#include <Arduino.h>

#include "space/animation.h"
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  Animation::begin();
  Serial.begin(115200);
  // Studpidty buffer
  delay(5000);
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
}

#include <Arduino.h>

#include "Core/Config.h"
#include "Core/LCD.h"
#include "Space/animation.h"
#include "core/Wifi.h"
/*---------------------------------------------------------------------------------------
 * Globals
 *-------------------------------------------------------------------------------------*/
Config config;
LCD lcd;
// Wifi wifi;
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  Animation::begin();
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(5000);
  // setupw();
  // lcd.begin();
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void serial();
void loop() {
  // Print FPS once every second
  static Timer print_interval = 0.1f;
  Animation::animate();
  // lcd.update();
  if (print_interval.update()) {
    static char fps[20];
    sprintf(fps, "FPS=%1.2f", Animation::fps());
    // lcd.print(fps);
  }
  // serial();
  // loopw();
}

void serial() {
  if (Serial1.available()) {
    Serial.write(Serial1.read());
  }

  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}
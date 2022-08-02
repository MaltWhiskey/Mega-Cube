#include <Arduino.h>

#include "core/Config.h"
#include "core/ESP8266.h"
#include "core/LCD.h"
#include "space/animation.h"
/*------------------------------------------------------------------------------
 * Globals
 *----------------------------------------------------------------------------*/
Config config;
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  // Start with clearing blue leds asap
  Animation::begin();
  // Serial output to usb for console display
  Serial.begin(115200);
  // ESP8266 UART baudrate on Hardware Serial1
  Serial1.begin(460800);
  // Prevents RX buffer overflow if not reading fast enough
  DMAMEM static char read_buffer[4096];
  Serial1.addMemoryForRead(read_buffer, sizeof(read_buffer));
  // Prevents TX buffer overflow and blocking the program
  DMAMEM static char write_buffer[1024];
  Serial1.addMemoryForWrite(write_buffer, sizeof(write_buffer));
  // Safety delay in case of code crash
  delay(2000);
  // Request time from Internet, the UART or Internet might fail
  ESP8266::request_time();
  delay(3000);
  LCD::begin();
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  // Print FPS once every x seconds
  static Timer print_interval = 10.0f;

  Animation::loop();
  ESP8266::loop();
  LCD::loop();

  if (print_interval.update()) {
    static char fps[20];
    sprintf(fps, "FPS=%1.2f", Animation::fps());
    Serial.println(fps);
    // Serial1.print(fps);
    // Serial1.print("\xf8\xfa");
    //  Serial1.flush();
    //   Testing connection from teensy to esp to wio terminal
    //   Send end esp and end wio commands to reset bit errors
    // static uint8_t byte = 0;
    // Serial1.printf("\xf8%02X ", byte++);
    // Serial1.flush();
  }
}
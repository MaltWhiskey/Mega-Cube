#ifndef ESP8266_H
#define ESP8266_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdint.h>
#include <string.h>

#include "Config.h"

class ESP8266 {
 private:
 protected:
 public:
  static void reset();
  static void loop();
  static void rpc(String msg);
  static void request_time();
};
#endif
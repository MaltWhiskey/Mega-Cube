#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <rpcWiFi.h>
#include <string.h>

class TCP {
 private:
  WiFiClient client;

  struct {
    char ssid[64] = "-^..^-";
    char password[64] = "qazwsxedc";
    char hostname[64] = "192.168.178.79";
    uint16_t tcp_port = 8000;
    uint16_t ota_port = 8266;
  } network;

 public:
  TCP();
  void begin();
  void loop();
  void rpc(String msg);

 private:
  bool connected();
};

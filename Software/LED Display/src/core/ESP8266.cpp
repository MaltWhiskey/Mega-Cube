#include "ESP8266.h"

#include <TimeLib.h>
// Start and stop bytes for commands (outside of ascii range)
const char ESP_START = 0xf0;
const char TEENSY_START = 0xf1;
const char WIO_START = 0xf2;
const char ESP_STOP = 0xf8;
const char TEENSY_STOP = 0xf9;
const char WIO_STOP = 0xfa;
const char START = TEENSY_START;
const char STOP = TEENSY_STOP;

void execute(const char* char_buffer);

void ESP8266::reset() {
  // ESP8266 reset pin, pull down for reset
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  delay(100);
  digitalWrite(14, HIGH);
  delay(100);
}

// Check input from Serial1 connected to ESP8266
void ESP8266::loop() {
  static boolean command = false;
  static String message = "";
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    if (c == START) {
      message = "";
      command = true;
    } else if (c == STOP) {
      if (command) {
        execute(message.c_str());
      }
      message = "";
      command = false;
    } else {
      if (command) {
        // Accumulate command until stop character
        message += c;
      } else {
        // Serial passthrough (USB console logger)
        Serial.write(c);
      }
    }
  }
}

// Executes a json command present in the char_buffer
void execute(const char* char_buffer) {
  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, char_buffer);
  if (err) {
    Serial.printf("Deserialization error: %s\n", err.c_str());
    return;
  }
  String event = doc["event"];
  // Serial.printf("Executing: %s\n", event.c_str());
  // Serial.println(char_buffer);
  if (event.equals("button")) {
  //  config.devices.joystick.x = doc["x"];
  //  config.devices.joystick.y = doc["y"];
 //   config.devices.joystick.z = doc["z"];
//    config.hid.button.a = doc["a"];
//    config.hid.button.b = doc["b"];
//    config.hid.button.c = doc["c"];
  } else if (event.equals("fft")) {
    String msg = doc["data"];
    for (uint16_t i = 0; i < msg.length(); i++) {
      char c = msg.charAt(i) - 'A';
  //    config.hid.fft.data[i] = (uint8_t)c;
   //   config.hid.fft.updated = true;
    }
  } else if (event.equals("time")) {
    Serial.println(char_buffer);
    uint32_t epoc = doc["epoc"];
    setTime(epoc);
  } else if (event.equals("accelerometer")) {
    //config.hid.accelerometer.x = doc["x"];
    //config.hid.accelerometer.y = doc["y"];
    //config.hid.accelerometer.z = doc["z"];
  }
}

void ESP8266::rpc(String msg) {
  Serial1.write(ESP_START);
  Serial1.print(msg);
  Serial1.write(ESP_STOP);
  Serial1.flush();
}

void ESP8266::request_time() {
  char buffer[256];
  StaticJsonDocument<256> doc;
  doc["event"] = "time";
  serializeJson(doc, buffer);
  rpc(buffer);
  doc.clear();
}
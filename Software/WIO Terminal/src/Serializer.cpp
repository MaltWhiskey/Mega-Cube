#include "Serializer.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LIS3DHTR.h>
extern LIS3DHTR<TwoWire> lis;

Serializer::Serializer(int m_bins, TCP* m_tcp) {
  this->m_bins = m_bins;
  this->m_tcp = m_tcp;
}
void Serializer::update(int* fft) {
  char buffer[1024];
  String msg = "";
  // NORMALLY bin zero has dc and mains hum, should ignore.
  // NORMALLY 2nd half of bins are not usable
  // THIS appox library i don't know and i don't care so lets
  // just ignore bin[0] and skip 2nd half > bin[m_bins/2]
  // Also m_bins = 512 so lets hard code it, to make it easy
  // i need 64 channels for nice 4x16x4 analyzer
  for (int i = 1; i < 256; i++) {
    // add 4 bins together or (3 for the first)
    static int32_t amp = 0;
    amp += fft[i];
    if ((i & 3) == 3) {
      uint8_t byte = (amp >> 10) & 0xF;
      if ((amp >> 10) > 0xf) byte = 0xf;
      byte += 'A';
      msg += (char)byte;
      amp = 0;
    }
  }
  StaticJsonDocument<1024> doc;
  doc["event"] = "fft";
  doc["data"] = msg;
  serializeJson(doc, buffer);
  m_tcp->rpc(buffer);
  doc.clear();

  doc["event"] = "button";
  if (digitalRead(WIO_5S_LEFT) == LOW) {
    doc["x"] = (int)-1;
  } else if (digitalRead(WIO_5S_RIGHT) == LOW) {
    doc["x"] = (int)1;
  } else {
    doc["x"] = (int)0;
  }
  if (digitalRead(WIO_5S_UP) == LOW) {
    doc["y"] = (int)1;
  } else if (digitalRead(WIO_5S_DOWN) == LOW) {
    doc["y"] = (int)-1;
  } else {
    doc["y"] = (int)0;
  }
  doc["z"] = (digitalRead(WIO_5S_PRESS) == LOW);
  doc["a"] = (digitalRead(WIO_KEY_A) == LOW);
  doc["b"] = (digitalRead(WIO_KEY_B) == LOW);
  doc["c"] = (digitalRead(WIO_KEY_C) == LOW);
  serializeJson(doc, buffer);
  m_tcp->rpc(buffer);
  doc.clear();

  doc["event"] = "accelerometer";
  doc["x"] = (float)lis.getAccelerationX();
  doc["y"] = (float)lis.getAccelerationY();
  doc["z"] = (float)lis.getAccelerationZ();
  serializeJson(doc, buffer);
  m_tcp->rpc(buffer);
  doc.clear();
}
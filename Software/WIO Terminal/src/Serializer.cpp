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

  doc["event"] = "move";
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
  if (btn_stick != (digitalRead(WIO_5S_PRESS) == LOW)) {
    btn_stick = !btn_stick;
    doc["press"] = btn_stick;
  }
  if (btn_a != (digitalRead(WIO_KEY_A) == LOW)) {
    btn_a = !btn_a;
    doc["a"] = btn_a;
  }
  if (btn_b != (digitalRead(WIO_KEY_B) == LOW)) {
    btn_b = !btn_b;
    doc["b"] = btn_b;
  }
  if (btn_c != (digitalRead(WIO_KEY_C) == LOW)) {
    btn_c = !btn_c;
    doc["c"] = btn_c;
  }
  serializeJson(doc, buffer);
  m_tcp->rpc(buffer);
  doc.clear();

  doc["event"] = "gyro";
  float x = lis.getAccelerationX();
  float y = lis.getAccelerationY();
  float z = lis.getAccelerationZ();
  doc["x"] = x;
  doc["y"] = y;
  doc["z"] = z;
  serializeJson(doc, buffer);
  m_tcp->rpc(buffer);
  doc.clear();
}
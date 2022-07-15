#include <Arduino.h>

#include "Application.h"
#ifdef USE_TFT
#include <TFT_eSPI.h>
#else
#include <LovyanGFX.hpp>
#endif
#include <LIS3DHTR.h>

#include "Display.h"

Application *application;

LIS3DHTR<TwoWire> lis;
#define USE_LGFX
//#define USE_TFT

void setup() {
  Serial.begin(115200);
  lis.begin(Wire1);
  // while (!Serial)
  //   ;
  delay(2000);
  Serial.println("Starting");
  if (!lis) Serial.println("ERROR starting LIS");
  lis.setOutputDataRate(LIS3DHTR_DATARATE_200HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G);

#ifdef USE_LGFX
  LGFX *lcd = new LGFX();
  lcd->begin();
  lcd->setColorDepth(16);
  lcd->setBrightness(50);
  lcd->setRotation(1);
  Display *display = new DisplayWrapper<LGFX>(*lcd);
#endif
#ifdef USE_TFT
  TFT_eSPI *lcd = new TFT_eSPI();
  lcd->begin();
  lcd->setRotation(3);
  Display *display = new DisplayWrapper<TFT_eSPI>(*lcd);
#endif
  application = new Application(*display);
  application->begin();
}

void loop() { application->loop(); }
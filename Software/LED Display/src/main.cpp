#include "Color.h"
#include "Display.h"

uint8_t wheel = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.print("F_CPU_ACTUAL=");
  Serial.println(F_CPU_ACTUAL);

  configurePll();
  configureFlexIO();
  configureDma();
}

void loop() {
  for (int channel = 0; channel < 32; channel++) {
    for (int led = 0; led < LEDCOUNT; led++) {
      Color c = Color(wheel + led, &RainbowGradientPalette[0]);
      setLed(channel, led, c.scale(64));
    }
  }
  wheel++;
  delayMicroseconds(5000);
}
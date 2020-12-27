#include "TDWS2811.h"

TDWS2811 td;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(0, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.println("Start Setup");
  td.setChannelType(31,GRBW);
  Serial.printf("Setup complete!\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.printf("Active buffer = %08X\t Inactive buffer = %08X",td.getActiveBuffer(),td.getInactiveBuffer());
  digitalWrite(13, !digitalRead(13));

  color_t baseColor={0,0,50,0};
  color_t foreColor={255,255,255,255};
  int i;
  for (i=0;i<250;i++) td.setLed(31,i,baseColor);
  for (i=0;i<250;i++)
  {
    td.setLed(31,i,foreColor);
    if (i>0) td.setLed(31,i-1,baseColor);
    delay(5);
  }
}

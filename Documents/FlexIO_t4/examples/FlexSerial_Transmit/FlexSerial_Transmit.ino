//============================================================================
// Simple output only test for Flex Serial pin defined on Pin 2
// Idea is to connect pin 0 (Serail1 RX pin) to Pin2 (Flex IO output)
// 
// And the program will hopefully echo the stuff to The serial output
//============================================================================

#include <FlexIO_t4.h>
#include <FlexSerial.h>

FlexSerial SerialFlex(-1, 2); // currently setup for pin 2 TX


void setup() {
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  while (!Serial && millis() < 4000);
  Serial.begin(115200);
  Serial1.begin(115200);  // lets start up Serial1, to see if we can receive anything from our FlexSerial
  delay(500);
  SerialFlex.begin(115200);

  Serial.println("End Setup");
}

uint8_t loop_char = 'a';
void loop() {
  digitalWrite(13, !digitalRead(13));
  SerialFlex.println("abcdefghijklmnopqrstuvwxyz");
  delay(500);

}

void serialEvent1() {
  int ch;
  while ((ch = Serial1.read()) != -1) {
    Serial.write(ch);
  }
}
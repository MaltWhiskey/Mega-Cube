//============================================================================
// Simple output only test for Flex Serial pin defined on Pin 2
// Idea is to connect pin 0 (Serail1 RX pin) to Pin2 (Flex IO output)
// 
// And the program will hopefully echo the stuff to The serial output
//============================================================================

#include <FlexIO_t4.h>
#include <FlexSerial.h>

#define TX_PIN 3
#define TX_FLEX_IO_INDEX 0
#define TX_FLEX_IO_TIMER 1
#define TX_FLEX_IO_SHIFTER 1
FlexSerial SerialFlex(-1, TX_PIN, -1, -1, -1, TX_FLEX_IO_INDEX, TX_FLEX_IO_TIMER, TX_FLEX_IO_SHIFTER);

int count_bytes_still_expected = 0;

void setup() {
  pinMode(13, OUTPUT);
  while (!Serial && millis() < 4000);
  Serial.begin(115200);
  Serial1.begin(115200);  // lets start up Serial1, to see if we can receive anything from our FlexSerial
  delay(500);
  SerialFlex.begin(115200);

  Serial.printf("Connect jumper from pin %d to 0)\n", TX_PIN);
  Serial.println("End Setup");
  count_bytes_still_expected = 32; // close enough...
  SerialFlex.println("Enter something to have it echo");
}


void loop() {
  int ch;
  if (Serial.available()) {
    count_bytes_still_expected = 0;
    digitalWrite(13, !digitalRead(13));
    while ((ch = Serial.read()) != -1){
      SerialFlex.write(ch);  
      count_bytes_still_expected++;
    }
  }
  delay(500);

}

void serialEvent1() {
  int ch;
  while ((ch = Serial1.read()) != -1) {
    if (count_bytes_still_expected > -10) {
      Serial.write(ch);
      count_bytes_still_expected--;
    } else if (count_bytes_still_expected == -10) {
      Serial.println("*** Error receiving too many bytes ***");
      count_bytes_still_expected--;
    }
  }
}

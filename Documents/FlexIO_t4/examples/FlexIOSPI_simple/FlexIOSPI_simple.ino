#include <FlexIO_t4.h>
#include <FlexIOSPI.h>

//#define HARDWARE_CS
#ifdef HARDWARE_CS
FlexIOSPI SPIFLEX(2, 3, 4, 5); // Setup on (int mosiPin, int sckPin, int misoPin, int csPin=-1) :
#define assert_cs()
#define release_cs()
#else
FlexIOSPI SPIFLEX(2, 3, 4, -1); // Setup on (int mosiPin, int sckPin, int misoPin, int csPin=-1) :
#define assert_cs() digitalWriteFast(5, LOW)
#define release_cs() digitalWriteFast(5, HIGH)
#endif

void setup() {
  pinMode(13, OUTPUT);
  while (!Serial && millis() < 4000);
  Serial.begin(115200);
  delay(500);
#ifndef HARDWARE_CS
  pinMode(5, OUTPUT);
  release_cs();
#endif
  if (!SPIFLEX.begin()) {
    Serial.println("SPIFlex Begin Failed");
  }

  // See if we can update the speed...
  //SPIFLEX.flexIOHandler()->setClockSettings(2, 1, 7);	// clksel(0-3PLL4, Pll3 PFD2 PLL5, *PLL3_sw)

  Serial.printf("Updated Flex IO speed: %u\n", SPIFLEX.flexIOHandler()->computeClockRate());

  Serial.println("End Setup");
}
uint8_t buf[] = "abcdefghijklmnopqrstuvwxyz";
uint16_t ret_buf[256];
uint8_t ch_out = 0;

void loop() {
  SPIFLEX.beginTransaction(FlexIOSPISettings(20000000, MSBFIRST, SPI_MODE0));
  assert_cs();
  for (uint8_t ch_out = 0; ch_out < 64; ch_out++) {
    ret_buf[ch_out] = SPIFLEX.transfer(ch_out);
  }
  release_cs();
  Serial.println("After Transfer loop");
  delay(25);

  uint8_t index = 0;
  assert_cs();
  for (uint16_t ch_out = 0; ch_out < 500; ch_out += 25) {
    ret_buf[index++] = SPIFLEX.transfer16(ch_out);
  }
  release_cs();
  Serial.println("After Transfer16 loop");
  delay(25);
  assert_cs();
  SPIFLEX.transfer(buf, NULL, sizeof(buf));
  release_cs();
  Serial.println("After Transfer buf");
  SPIFLEX.endTransaction();
  delay(500);
}
// Note: This will only work for T4.x as 
// T3.5 or T3.6 does not have enough memory for two buffers
#include "SPI.h"
#include "ILI9341_t3n.h"


#define TFT_DC  9
#define TFT_CS  10
#define TFT_RST 8
#define CENTER ILI9341_t3n::CENTER
DMAMEM uint16_t fb1[320 * 240];
DMAMEM uint16_t fb2[320 * 240];

ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);
void setup() {
  while (!Serial && millis() < 5000) ; // wait for Arduino Serial Monitor
  Serial.begin(9600);
  delay(500);
  Serial.println("ILI9341 Test!");
  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.fillScreen(ILI9341_GREEN);
  tft.fillScreen(ILI9341_RED);
  tft.fillScreen(ILI9341_WHITE);
  tft.fillScreen(ILI9341_BLUE);

  tft.setFrameBuffer(fb1);
  tft.useFrameBuffer(true);
  tft.fillScreen(ILI9341_GREEN);
  tft.setCursor(CENTER, CENTER);
  tft.setTextColor(ILI9341_RED);
  tft.print("Using FB1");
  uint32_t start_time = micros();
  tft.updateScreen();
  Serial.printf("Delta time: %u\n", micros() - start_time);
  waitUserInput();
  tft.setFrameBuffer(fb2);
  tft.fillScreen(ILI9341_BLUE);
  tft.setCursor(CENTER, CENTER);
  tft.setTextColor(ILI9341_RED);
  tft.print("Using FB2");
  start_time = micros();
  tft.updateScreen();
  Serial.printf("Delta time: %u\n", micros() - start_time);
  waitUserInput();
  tft.useFrameBuffer(fb1);
  tft.fillScreen(ILI9341_GREEN);
  tft.setCursor(CENTER, CENTER);
  tft.setTextColor(ILI9341_RED);
  tft.print("Using FB1A");
  start_time = micros();
  tft.updateScreenAsync();
  tft.waitUpdateAsyncComplete();
  Serial.printf("Delta time: %u\n", micros() - start_time);

  waitUserInput();
  tft.useFrameBuffer(fb2);
  tft.setFrameBuffer(fb2);
  tft.fillScreen(ILI9341_BLUE);
  tft.setCursor(CENTER, CENTER);
  tft.setTextColor(ILI9341_RED);
  tft.print("Using FB2A");
  start_time = micros();
  tft.updateScreenAsync();
  tft.waitUpdateAsyncComplete();
  Serial.printf("Delta time: %u\n", micros() - start_time);
}

void loop() {
  waitUserInput();
  for (uint16_t row = 0; row < 240; row++) {
    Serial.print(".");
    if (!(row & 63)) Serial.println();
    tft.setFrameBuffer(row & 1 ? fb1 : fb2);
    tft.fillScreen(ILI9341_BLACK);
    tft.fillRect(0, 0, tft.width(), row, ILI9341_GREEN);
#if 1
    elapsedMillis em = 0;
    while (tft.asyncUpdateActive() && ( em < 100)) ;
    if (em >= 100) { Serial.print("Timeout"); Serial.flush(); }
//    tft.waitUpdateAsyncComplete();
    tft.updateScreenAsync();
#else    
    tft.updateScreenAsync();
    tft.waitUpdateAsyncComplete();
#endif
  }
  Serial.println();
  tft.waitUpdateAsyncComplete();
}

void waitUserInput() {
  Serial.println("Press any key to continue");
  while (Serial.read() == -1) ;
  while (Serial.read() != -1);
  Serial.println("Continued");
}

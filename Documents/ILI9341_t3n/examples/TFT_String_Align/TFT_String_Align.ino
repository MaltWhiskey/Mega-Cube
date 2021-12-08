/*
Tests string alignment

Normally strings are printed relative to the top left corner but this can be
changed with the setTextDatum() function. The library has #defines for:

TL_DATUM = Top left
TC_DATUM = Top centre
TR_DATUM = Top right
ML_DATUM = Middle left
MC_DATUM = Middle centre
MR_DATUM = Middle right
BL_DATUM = Bottom left
BC_DATUM = Bottom centre
BR_DATUM = Bottom right
*/

#include <ILI9341_t3n.h> // Hardware-specific library
#include <SPI.h>
#include <ili9341_t3n_font_Arial.h>

#define ILI9341_RST 23
#define ILI9341_DC 9
#define ILI9341_CS 10
ILI9341_t3n tft = ILI9341_t3n(ILI9341_CS, ILI9341_DC, ILI9341_RST);

unsigned long drawTime = 0;

void setup(void) {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.setFont(Arial_18);
  //tft.setTextSize(4);
}

void loop() {

  tft.fillScreen(ILI9341_BLACK);
  
  for(byte datum = 0; datum < 9; datum++) {
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    
    tft.setTextDatum(datum);
    
    tft.drawNumber(88,160,60);
    tft.fillCircle(160,120,5,ILI9341_RED);
    
    tft.setTextDatum(MC_DATUM);
    
    tft.setTextColor(ILI9341_YELLOW);
    tft.drawString("TEENSY 4",160,120);
    delay(1000);
    tft.fillScreen(ILI9341_BLACK);
  }

  tft.setTextDatum(MC_DATUM);
  
  tft.setTextColor(ILI9341_BLACK);
  tft.drawString("X",160,120);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setTextDatum(MC_DATUM);
  
  tft.setTextColor(ILI9341_BLACK);
  tft.drawString("X",160,120);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);

  tft.setTextDatum(MC_DATUM);

  //Test floating point drawing function
  float test = 67.125;
  tft.drawFloat(test, 4, 160, 180);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  test = -0.555555;
  tft.drawFloat(test, 3, 160, 180);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  test = 0.1;
  tft.drawFloat(test, 4, 160, 180);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  test = 9999999;
  tft.drawFloat(test, 1, 160, 180);
  delay(1000);
  
  tft.fillCircle(160,180,5,ILI9341_YELLOW);
  
  tft.setTextDatum(MC_DATUM);
  
  tft.setTextColor(ILI9341_BLACK);
  tft.drawString("X",160,180);

  delay(4000);
}

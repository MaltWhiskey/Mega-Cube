/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <SPI.h>
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_ComicSansMS.h>

// For the Adafruit shield, these are the default.
#define ILI9341_RST 8
#define ILI9341_DC 9
#define ILI9341_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3n tft = ILI9341_t3n(ILI9341_CS, ILI9341_DC, ILI9341_RST);

// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {

  Serial.begin(9600);


 
  tft.begin();
  tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);
  while (!Serial) ; 
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.enableScroll();
  tft.setScrollTextArea(0,0,120,240);
  tft.setScrollBackgroundColor(ILI9341_GREEN);

  tft.setCursor(180, 100);

  tft.setFont(ComicSansMS_12);
  tft.print("Fixed text");

  tft.setCursor(0, 0);

  tft.setTextColor(ILI9341_BLACK); 

  for(int i=0;i<20;i++){
    tft.print("  this is line ");
    tft.println(i);
    delay(100);
  }

  tft.fillScreen(ILI9341_BLACK);
  tft.setScrollTextArea(40,50,120,120);
  tft.setScrollBackgroundColor(ILI9341_GREEN);
  tft.setFont(ComicSansMS_10);

  tft.setTextSize(1);
  tft.setCursor(40, 50);

  for(int i=0;i<20;i++){
    tft.print("  this is line ");
    tft.println(i);
    delay(500);
  }


}



void loop(void) {


}

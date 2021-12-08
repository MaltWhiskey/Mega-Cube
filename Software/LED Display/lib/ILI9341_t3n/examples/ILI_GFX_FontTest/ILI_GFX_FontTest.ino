#include <Adafruit_GFX.h>

#include <SPI.h>
#include <ILI9341_t3n.h>

#include <ili9341_t3n_font_ComicSansMS.h>
#include "Fonts/FreeSansOblique12pt7b.h"

#define TFT_RST 8
#define TFT_DC  9  // only CS pin 
#define TFT_CS  10   // using standard pin
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("Setup");
  tft.begin();

  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_WHITE);
  tft.println("Test of the default font");
  tft.println();

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
  tft.setFont(ComicSansMS_12);
  tft.println("Opaque ILI font ComicSansMS_12");
  tft.println();

  tft.setTextColor(ILI9341_GREEN);
  tft.println("Transparent ILIComicSansMS_12");
  tft.println();

  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
  tft.setFont(&FreeSansOblique12pt7b);
  tft.println("Opaque GFX FreeSansOblique12pt");
  int cursor_x = tft.getCursorX();
  int cursor_y = tft.getCursorY();
  tft.fillRect(cursor_x, cursor_y, tft.width(), 20, ILI9341_YELLOW);
  tft.setTextColor(ILI9341_BLUE);
  tft.println("Transparent GFX");

  tft.setFont();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("This is default font:");
  //tft.setFontSpacing(1);//now give 5 pix extra spacing between chars
  tft.println("ABCDEF 1 2 3 4 5 6 7");

}

void loop()
{  }

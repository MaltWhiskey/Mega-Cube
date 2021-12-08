#include <Adafruit_GFX.h>

#include <SPI.h>
#include <ILI9341_t3n.h>

#include "font_Arial.h"
#include "font_ArialBold.h"
#include "font_ComicSansMS.h"
#include "font_OpenSans.h"
#include "font_DroidSans.h"
#include "font_Michroma.h"
#include "font_Crystal.h"
#include "font_ChanceryItalic.h"

#define CENTER ILI9341_t3n::CENTER

// maybe a few GFX FOnts?
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

typedef struct {
  const ILI9341_t3_font_t *ili_font;
  const GFXfont       *gfx_font;
  const char          *font_name;
  uint16_t            font_fg_color;
  uint16_t            font_bg_color;
} ili_fonts_test_t;


const ili_fonts_test_t font_test_list[] = {
  {&Arial_12, nullptr,  "Arial_12", ILI9341_WHITE, ILI9341_WHITE},
  {&Arial_12_Bold, nullptr,  "ArialBold 12", ILI9341_YELLOW, ILI9341_YELLOW},
  {&ComicSansMS_12, nullptr,  "ComicSansMS 12", ILI9341_GREEN, ILI9341_GREEN},
  {&DroidSans_12, nullptr,  "DroidSans_12", ILI9341_WHITE, ILI9341_WHITE},
  {&Michroma_12, nullptr,  "Michroma_12", ILI9341_YELLOW, ILI9341_YELLOW},
  {&Crystal_16_Italic, nullptr,  "CRYSTAL_16", ILI9341_BLACK, ILI9341_YELLOW},
  {&Chancery_16_Italic, nullptr,  "Chancery_16_Italic", ILI9341_GREEN, ILI9341_GREEN},
  {&OpenSans16, nullptr,  "OpenSans 16", ILI9341_RED, ILI9341_YELLOW},
  {nullptr, &FreeMono9pt7b,  "GFX FreeMono9pt7b", ILI9341_WHITE, ILI9341_WHITE},
  {nullptr, &FreeMono9pt7b,  "GFX FreeMono9pt7b", ILI9341_RED, ILI9341_YELLOW},
  {nullptr, &FreeSerif9pt7b,  "GFX FreeSerif9pt7b", ILI9341_WHITE, ILI9341_WHITE},
  {nullptr, &FreeSerif9pt7b,  "GFX FreeSerif9pt7b", ILI9341_RED, ILI9341_YELLOW},

} ;

// *************** Change to your Pin numbers ***************
#define TFT_DC  9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_SCK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TOUCH_CS  6

ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

uint8_t test_screen_rotation = 0;


void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("Setup");
  tft.begin();

  tft.fillWindow(ILI9341_BLACK);

  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(Arial_12);
  tft.println("Arial_12");
  displayStuff();

  tft.setTextColor(ILI9341_YELLOW);
  tft.setFont(Arial_12_Bold);
  tft.println("ArialBold 12");
  displayStuff();
  nextPage();
  tft.setTextColor(ILI9341_GREEN);
  tft.setFont(ComicSansMS_12);
  tft.println("ComicSansMS 12");
  displayStuff();


  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(DroidSans_12);
  tft.println("DroidSans_12");
  displayStuff();
  nextPage();

  tft.setTextColor(ILI9341_YELLOW);
  tft.setFont(Michroma_12);
  tft.println("Michroma_12");
  displayStuff();

  tft.setTextColor(ILI9341_BLACK, ILI9341_YELLOW);
  tft.setFont(Crystal_16_Italic);
  tft.println("CRYSTAL_16");
  displayStuff();

  nextPage();

  tft.setTextColor(ILI9341_GREEN);
  tft.setFont(Chancery_16_Italic);
  tft.println("Chancery_16_Italic");
  displayStuff();

  //anti-alias font OpenSans
  tft.setTextColor(ILI9341_RED, ILI9341_YELLOW);
  tft.setFont(OpenSans16);
  tft.println("OpenSans 18");
  displayStuff();

  Serial.println("Basic Font Display Complete");
  Serial.println("Loop test for alt colors + font");
}

void loop()
{
  tft.setFont(Arial_12);
  Serial.printf("\nRotation: %d\n", test_screen_rotation);
  tft.setRotation(test_screen_rotation);
  tft.fillWindow(ILI9341_RED);
  tft.setCursor(CENTER, CENTER);
  tft.printf("Rotation: %d", test_screen_rotation);
  test_screen_rotation = (test_screen_rotation + 1) & 0x3;
  /*  tft.setCursor(200, 300);
    Serial.printf("  Set cursor(200, 300), retrieved(%d %d)",
                  tft.getCursorX(), tft.getCursorY());
  */
  tft.setCursor(25, 25);
  tft.write('0');
  tft.setCursor(tft.width() - 25, 25);
  tft.write('1');
  tft.setCursor(25, tft.height() - 25);
  tft.write('2');
  tft.setCursor(tft.width() - 25, tft.height() - 25);
  tft.write('3');

  for (uint8_t font_index = 0; font_index < (sizeof(font_test_list) / sizeof(font_test_list[0])); font_index++) {
    nextPage();
    if (font_test_list[font_index].font_fg_color != font_test_list[font_index].font_bg_color)
      tft.setTextColor(font_test_list[font_index].font_fg_color, font_test_list[font_index].font_bg_color);
    else
      tft.setTextColor(font_test_list[font_index].font_fg_color);
    if (font_test_list[font_index].ili_font) tft.setFont(*font_test_list[font_index].ili_font);
    else tft.setFont(font_test_list[font_index].gfx_font);
    tft.println(font_test_list[font_index].font_name);
    Serial.println(font_test_list[font_index].font_name);
    displayStuff1();
  }
  nextPage();
}

uint32_t displayStuff()
{
  elapsedMillis elapsed_time = 0;
  tft.println("ABCDEFGHIJKLM");
  tft.println("nopqrstuvwxyz");
  tft.println("0123456789");
  tft.println("!@#$%^ &*()-");
  tft.println(); tft.println();
  return (uint32_t) elapsed_time;
}

uint32_t displayStuff1()
{
  elapsedMillis elapsed_time = 0;
  tft.println("ABCDEFGHIJKLM");
  tft.println("nopqrstuvwxyz");
  tft.println("0123456789");
  tft.println("!@#$%^ &*()-");

  int16_t cursorX = tft.getCursorX();
  int16_t cursorY = tft.getCursorY();

  uint16_t width = tft.width();
  uint16_t height = tft.height();
  Serial.printf("DS1 (%d,%d) %d %d\n", cursorX, cursorY, width, height);
  uint16_t rect_x = width / 2 - 50;
  uint16_t rect_y = height - 50;
  tft.drawRect(rect_x, rect_y, 100, 40, ILI9341_WHITE);
  for (uint16_t y = rect_y + 5; y < rect_y + 40; y += 5)
    tft.drawFastHLine(rect_x + 1, y, 98, ILI9341_PINK);
  for (uint16_t x = rect_x + 5; x < rect_x + 100; x += 5)
    tft.drawFastVLine(x, rect_y + 1, 38, ILI9341_PINK);
  tft.setCursor(width / 2, height - 30, true);
  tft.print("Center");

  // Lets try again with CENTER X keyword.
  rect_y -= 60;
  tft.drawRect(rect_x, rect_y, 100, 40, ILI9341_PINK);
  for (uint16_t y = rect_y + 5; y < rect_y + 40; y += 5)
    tft.drawFastHLine(rect_x + 1, y, 98, ILI9341_CYAN);
  for (uint16_t x = rect_x + 5; x < rect_x + 100; x += 5)
    tft.drawFastVLine(x, rect_y + 1, 38, ILI9341_CYAN);
  tft.setCursor(CENTER, rect_y);
  tft.print("XCENTR");

  // Lets try again with CENTER Y keyword.
  rect_x = 50;
  rect_y = tft.height() / 2 - 25;
  tft.drawRect(rect_x, rect_y, 100, 50, ILI9341_CYAN);
  for (uint16_t y = rect_y + 5; y < rect_y + 50; y += 5)
    tft.drawFastHLine(rect_x + 1, y, 98, ILI9341_PINK);
  for (uint16_t x = rect_x + 5; x < rect_x + 100; x += 5)
    tft.setCursor(50, CENTER);
  tft.print("YCENTR");



  tft.setCursor(cursorX, cursorY);
  static const char alternating_text[] = "AbCdEfGhIjKlM\rNoPqRsTuVwXyZ";

  for (uint8_t i = 0; i < (sizeof(alternating_text) - 1); i++) {
    if (i & 1) tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    else tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
    tft.write(alternating_text[i]);
  }

  tft.println(); tft.println();



  return (uint32_t) elapsed_time;
}

void nextPage()
{
  Serial.println("Press anykey to continue");
  while (Serial.read() == -1) ;
  while (Serial.read() != -1) ;

  tft.fillWindow(ILI9341_BLACK);
  tft.setCursor(0, 0);
}

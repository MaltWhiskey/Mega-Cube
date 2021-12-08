#include <Adafruit_GFX.h>    // Core graphics library
#include <ili9341_t3n_font_Arial.h>
#include <ili9341_t3n_font_ArialBold.h>
#include <ILI9341_t3n.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#define ROTATION 3

#include "SPI.h"
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)  // Teensy 4.x 
#define TFT_DC  9  // only CS pin 
#define TFT_CS 10  // using standard pin
#define TFT_RST 8
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST);
#else
//#define DEFAULT_PINS
//#define USE_SPI1
#define KURTS_FLEXI
//#define FRANKS_C64

#ifdef KURTS_FLEXI
#define TFT_DC 22
#define TFT_CS 15
#define TFT_RST -1
#define TFT_SCK 14
#define TFT_MISO 12
#define TFT_MOSI 7
#define DEBUG_PIN 13

#elif defined(FRANKS_C64)
#define SCK       14
#define MISO      39
#define MOSI      28
#define TFT_TOUCH_CS    38
#define TFT_TOUCH_INT   37
#define TFT_DC          20
#define TFT_CS          21
#define TFT_RST        -1  // 255 = unused, connected to 3.3V
#define TFT_SCK        SCK
#define TFT_MOSI        MOSI
#define TFT_MISO        MISO

#elif defined(DEFAULT_PINS)
#define TFT_DC  9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_SCK 13
#define TFT_MISO 12
#define TFT_MOSI 11

#elif defined(USE_SPI1)
#define TFT_DC 31
#define TFT_CS 10 // any pin will work not hardware
#define TFT_RST 8
#define TFT_SCK 32
#define TFT_MISO 5
#define TFT_MOSI 21
#define DEBUG_PIN 13
#else
//#define TFT_DC  9
#define TFT_DC 45
#define TFT_CS 10
#define TFT_RST 7
#define TFT_SCK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#endif
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);
#endif

Adafruit_GFX_Button button;

// Let's allocate the frame buffer ourself.
DMAMEM uint16_t tft_frame_buffer[ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT];

uint8_t use_dma = 0;
uint8_t use_clip_rect = 0;
uint8_t use_set_origin = 0;
uint8_t use_fb = 0;

#define ORIGIN_TEST_X 50
#define ORIGIN_TEST_Y 50

void setup() {
  while (!Serial && (millis() < 4000)) ;
  Serial.begin(115200);
  //Serial.printf("Begin: CS:%d, DC:%d, MOSI:%d, MISO: %d, SCK: %d, RST: %d\n", TFT_CS, TFT_DC, TFT_MOSI, TFT_MISO, TFT_SCK, TFT_RST);

  tft.begin();
  tft.setFrameBuffer(tft_frame_buffer);

  tft.setRotation(ROTATION);
  tft.fillScreen(ILI9341_BLACK);

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
#ifdef DEBUG_PIN
  pinMode(DEBUG_PIN, OUTPUT);
#endif

  button.initButton(&tft, 200, 125, 100, 40, ILI9341_GREEN, ILI9341_YELLOW, ILI9341_RED, "UP", 1, 1);

  drawTestScreen();
}

void SetupOrClearClipRectAndOffsets() {
  if (use_clip_rect) {
    tft.setClipRect();  // make sure we clear the whole screen
    tft.setOrigin();    // make sure none are set yet

    tft.fillScreen(ILI9341_LIGHTGREY);

    // Now lets set origin.
    if (use_set_origin)
      tft.setOrigin(ORIGIN_TEST_X, ORIGIN_TEST_Y);
    int x = tft.width() / 4;
    int y = tft.height() / 4;
    int w = tft.width() / 2;
    int h = tft.height() / 2;
    tft.drawRect(x, y, w, h, ILI9341_ORANGE);
    tft.updateScreen();
    tft.setClipRect(x + 1, y + 1, w - 2, h - 2);
    delay(250);

  } else {
    tft.setClipRect();
    if (use_set_origin)
      tft.setOrigin(ORIGIN_TEST_X, ORIGIN_TEST_Y);
    else
      tft.setOrigin();
  }
}


uint16_t palette[256];  // Should probably be 256, but I don't use many colors...
uint16_t pixel_data[2500];
const uint8_t pict1bpp[] = {0xff, 0xff, 0xc0, 0x03, 0xa0, 0x05, 0x90, 0x9, 0x88, 0x11, 0x84, 0x21, 0x82, 0x41, 0x81, 0x81,
                            0x81, 0x81, 0x82, 0x41, 0x84, 0x21, 0x88, 0x11, 0x90, 0x09, 0xa0, 0x05, 0xc0, 0x03, 0xff, 0xff
                           };
const uint8_t pict2bpp[] = {
  0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff,
  0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff,
  0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 
  0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 
  0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 
  0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 
  0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 
  0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 
  0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff,
  0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff,
  0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 
  0x55, 0x55, 0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 
  0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 
  0xaa, 0xaa, 0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 
  0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 
  0xff, 0xff, 0x00, 0x00, 0x55, 0x55, 0xaa, 0xaa, 
};
const uint8_t pict4bpp[] = {  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,  
  0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,  
  0x00, 0x11, 0x22, 0x22, 0x22, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x22, 0x22, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x33, 0x33, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x33, 0x33, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x33, 0x33, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x33, 0x33, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x22, 0x22, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x22, 0x22, 0x22, 0x22, 0x11, 0x00,
  0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,  
  0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


                           
void drawTestScreen() {
  Serial.printf("Use FB: %d ", use_fb); Serial.flush();
  tft.useFrameBuffer(use_fb);
  SetupOrClearClipRectAndOffsets();
  uint32_t start_time = millis();
  tft.fillScreen(use_fb ? ILI9341_RED : ILI9341_BLACK);
  //tft.setFont(Inconsolata_60);
  tft.setFont(Arial_24_Bold);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 0);
  tft.println("Test");
  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
  tft.println("text");
  tft.setCursor(85, 65);
  tft.print("XYZ");
  tft.setFontAdafruit();
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.println("01234");
  tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
  tft.println("56789!@#$%");

  tft.drawRect(0, 150, 100, 50, ILI9341_WHITE);
  tft.drawLine(0, 150, 100, 50, ILI9341_GREEN);
  tft.fillRectVGradient(125, 150, 50, 50, ILI9341_GREEN, ILI9341_YELLOW);
  tft.fillRectHGradient(200, 150, 50, 50, ILI9341_YELLOW, ILI9341_GREEN);
  // Try a read rect and write rect
#ifdef DEBUG_PIN
  digitalWrite(DEBUG_PIN, HIGH);
#endif

  tft.readRect(0, 0, 50, 50, pixel_data);
  // For heck of it lets make sure readPixel and ReadRect 
  // give us same data, maybe check along diagnal?
  for (uint i=0; i < 50; i++) {
    uint16_t pixel_color = tft.readPixel(i,i);
    if (pixel_color != pixel_data[i*50+i]) {
      Serial.printf("Read rect/pixel mismatch: %d %x %x\n", i, pixel_color,pixel_data[i*50+i]);
    }    
  }

#ifdef DEBUG_PIN
  digitalWrite(DEBUG_PIN, LOW);
#endif
  tft.writeRect(250, 0, 50, 50, pixel_data);

  // Lets try to pack this rectangle of data into 8 byte
  tft.readRect(85, 65, 50, 50, pixel_data);
  uint16_t *ppd16 = pixel_data;
  uint8_t *ppd8 = (uint8_t*)pixel_data;
  uint8_t palette_cnt = 0;
  int palette_index;
  for (int i = 0; i < 2500; i++) {
    for (palette_index = 0; palette_index < palette_cnt; palette_index++) {
      if (*ppd16 == palette[palette_index])
        break;
    }
    if (palette_index >= palette_cnt) {
      palette[palette_cnt++] = *ppd16;  // save away the color
    }
    *ppd8++ = palette_index;
    ppd16++;
  }
  tft.writeRect8BPP(200, 50, 50, 50, (uint8_t*)pixel_data, palette);
  palette[0] = ILI9341_CYAN; 
  palette[1] = ILI9341_OLIVE; 
  tft.writeRect1BPP(75, 100, 16, 16, pict1bpp, palette);
  tft.writeRect1BPP(320 - 90, 75, 16, 16, pict1bpp, palette);
  
  palette[2] = ILI9341_MAROON; 
  palette[3] = ILI9341_PINK; 
  tft.writeRect2BPP(75, 125, 32, 16, pict2bpp, palette);

  tft.writeRectNBPP(15, 125, 32, 16, 2, pict2bpp, palette);
  tft.writeRectNBPP(75, 150, 16, 16, 4, pict4bpp, palette);

  // Try drawing button
  tft.setFontAdafruit();
  button.drawButton();

  if (use_dma) {
    tft.updateScreenAsync();
  } else {
  tft.updateScreen();
  }
  Serial.println(millis() - start_time, DEC);

  if (use_dma && use_fb) {
    delay(500);
    Serial.printf("DMA error status: %x\n", DMA_ES);
  }

  use_fb = use_fb ? 0 : 1 ;
  Serial.println(use_fb, DEC);


}

void fillScreenTest() {
  tft.useFrameBuffer(0);
  SetupOrClearClipRectAndOffsets();

  tft.fillScreen(ILI9341_RED);
  WaitForUserInput();
  tft.fillScreen(ILI9341_GREEN);
  WaitForUserInput();
  tft.fillScreen(ILI9341_WHITE);
  WaitForUserInput();
  tft.fillScreen(ILI9341_BLACK);

}
void printTextSizes(const char *sz) {
  Serial.printf("%s(%d,%d): SPL:%u ", sz, tft.getCursorX(), tft.getCursorY(), tft.strPixelLen(sz));
  int16_t x, y;
  uint16_t w, h;
  tft.getTextBounds(sz, tft.getCursorX(), tft.getCursorY(), &x, &y, &w, &h);
  Serial.printf(" Rect(%d, %d, %u %u)\n", x, y, w, h);  
  tft.drawRect(x, y, w, h, ILI9341_GREEN);
}


void drawTextScreen(bool fOpaque) {
  SetupOrClearClipRectAndOffsets();
  tft.setTextSize(1);
  uint32_t start_time = millis();
  tft.useFrameBuffer(use_fb);
  tft.fillScreen(use_fb ? ILI9341_RED : ILI9341_BLACK);
  tft.setFont(Arial_28_Bold);
//t  tft.setFont(Arial_40_Bold);
  if (fOpaque)
    tft.setTextColor(ILI9341_WHITE, use_fb ? ILI9341_BLACK : ILI9341_RED);
  else
    tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 5);
  tft.println("AbCdEfGhIj");
#if 0
  tft.setFont(Arial_28_Bold);
  tft.println("0123456789!@#$");
  tft.setFont(Arial_20_Bold);
  tft.println("abcdefghijklmnopq");
  tft.setFont(Arial_14_Bold);
  tft.println("ABCDEFGHIJKLMNOPQRST");
  tft.setFont(Arial_10_Bold);
  tft.println("0123456789zyxwvutu");
#endif
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  printTextSizes("AdaFruit_MB_12");
  if (fOpaque){
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("A");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.print("d");
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("a");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.print("F");
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("R");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.print("u");
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("i");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.print("t");
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("_");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.print("M");
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("B");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.print("_");
    tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
    tft.print("1");
    tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
    tft.println("2");
    tft.setTextColor(ILI9341_WHITE, use_fb ? ILI9341_BLACK : ILI9341_RED);
  }
  else tft.println("AdaFruit_MB_12");
  tft.setFont(&FreeSerif12pt7b);
  printTextSizes("FreeSan12");
  tft.println("FreeSan12");
  tft.println();
  tft.setTextSize(1,3);
  printTextSizes("Size 1,3");
  tft.println("Size 1,3");
  tft.setFont();
  tft.setCursor(0, 190);
  tft.setTextSize(1,2);
  printTextSizes("Sys(1,2)");
  tft.println("Sys(1,2)");
  tft.setTextSize(1);
  printTextSizes("System");
  tft.println("System");
  tft.setTextSize(1);


  tft.updateScreen();
  Serial.printf("Use FB: %d OP: %d, DT: %d OR: %d\n", use_fb, fOpaque, use_set_origin, millis() - start_time);
}


void drawGFXTextScreen(bool fOpaque) {
  SetupOrClearClipRectAndOffsets();
  tft.setTextSize(1);
  tft.setCursor(0, 10);
  if (fOpaque)
    tft.setTextColor(ILI9341_WHITE, use_fb ? ILI9341_BLACK : ILI9341_RED);
  else
    tft.setTextColor(ILI9341_WHITE);
  uint32_t start_time = millis();
  tft.useFrameBuffer(use_fb);
  tft.fillScreen(use_fb ? ILI9341_RED : ILI9341_BLACK);
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.println("MonoBold");
  tft.println("ABCDEFGHIJKLMNO");
  tft.println("abcdefghijklmno");
  tft.println("0123456789!@#$%^&*()_");
  tft.setFont(&FreeSerif12pt7b);
  tft.println("Serif12");
  tft.println("ABCDEFGHIJKLMNO");
  tft.println("abcdefghijklmno");
  tft.println("0123456789!@#$%^&*()_");
  tft.updateScreen();
  tft.setTextSize(1);
  tft.setFont();
  Serial.printf("Use FB: %d OP: %d, DT: %d\n", use_fb, fOpaque, millis() - start_time);
}
//=============================================================================
// Wait for user input
//=============================================================================
void WaitForUserInput() {
  Serial.println("Hit Enter to continue");
  Serial.flush();
  while (Serial.read() == -1) ;
  while (Serial.read() != -1) ;

}

//=============================================================================
// Try continuous update
//=============================================================================
void WaitForFrame(bool fCont, uint32_t wait_frame_count) {
  if (fCont) {
    while (tft.frameCount() < wait_frame_count) yield();
  } else {
    tft.updateScreenAsync();
    WaitForUserInput();
  }
}

void testDMAContUpdate(bool fCont) {
  // Force frame buffer on

  Serial.printf("continuous DMA udpate test - Frame mode on\n"); Serial.flush();
  if (!fCont) {
    Serial.println("Step Mode");
    Serial.flush();
  }
  use_fb = 1; //

  tft.useFrameBuffer(use_fb);
  tft.fillScreen(ILI9341_GREEN);

  // check to see if screen memory actually turned green.
  if (use_fb) {
    uint16_t *pw = tft.getFrameBuffer();
    int error_count = 0;
    for (int i = 0; i < (ILI9341_TFTWIDTH * ILI9341_TFTHEIGHT); i++)
    {
      if (*pw != ILI9341_GREEN) {
        Serial.printf("tft.fillScreen(ILI9341_GREEN) not green? %d != %x\n", i, *pw);
        error_count++;
      }
      pw++;
    }
    Serial.printf("tft.fillScreen(ILI9341_GREEN(%x)) error count = %d\n", ILI9341_GREEN, error_count);
  }

  if (fCont)
    tft.updateScreenAsync(fCont);

  // Start the update
  WaitForFrame(fCont, 10);

  tft.fillScreen(ILI9341_YELLOW);
  tft.drawRect(5, 5, 310, 230, ILI9341_GREEN);
  tft.fillRect(140, 100, 40, 40, ILI9341_BLUE);
  WaitForFrame(fCont, 20);

  tft.fillScreen(ILI9341_RED);
  tft.drawRect(5, 5, 310, 230, ILI9341_WHITE);

  WaitForFrame(fCont, 30);

  tft.fillScreen(ILI9341_BLACK);

  tft.drawRect(5, 5, 310, 230, ILI9341_GREEN);
  tft.drawRect(25, 25, 270, 190, ILI9341_RED);
  WaitForFrame(fCont, 40);

  digitalWrite(0, HIGH);
  tft.drawRect(5, 5, 310, 230, ILI9341_GREEN);
  tft.setCursor(10, 100);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setFont(Arial_20_Bold);
  tft.println("DONE");
  tft.setFont();
  tft.setCursor(10, 200);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("Done");
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
  tft.print("Done");
  digitalWrite(0, LOW);
  WaitForFrame(fCont, 45);
  tft.fillRect(0, 0, 2, 2, ILI9341_PURPLE);

  if (!fCont) {
    Serial.println("Lets now try doing Continue for a few iterations to see if it changes");
    tft.updateScreenAsync(true);
    while (tft.frameCount() < 10) yield();
  }
  tft.endUpdateAsync();
  Serial.println("after endUpdateAsync");
  tft.waitUpdateAsyncComplete();
  Serial.println("after waitUpdateAsyncComplete");
  Serial.println("Finished test");

  delay(2000);
  Serial.println("Do normal update to see if data is there");
  tft.updateScreen();

}

void loop(void) {
  // See if any text entered
  int ich;
  if ((ich = Serial.read()) != -1) {
    while (Serial.read() != -1) delay(1);

    // See if We have a dma operation in progress?
    if (tft.asyncUpdateActive()) {
      Serial.printf("Async Update active DMA error status: %x\n", DMA_ES);
      //tft.dumpDMASettings();
    }

    if (ich == 'c') {
      use_clip_rect = !use_clip_rect;
      if (use_clip_rect) Serial.println("Clip Rectangle Turned on");
      else Serial.println("Clip Rectangle turned off");
      return;
    }
    if (ich == 'd') {
      use_dma = !use_dma;
      if (use_dma) Serial.println("DMA Turned on");
      else Serial.println("DMA turned off");
      return;
    }

    if (ich == 's') {
      use_set_origin = !use_set_origin;
      if (use_set_origin) Serial.printf("Set origin to %d, %d\n", ORIGIN_TEST_X, ORIGIN_TEST_Y);
      else Serial.println("Clear origin");
      return;
    }
    if (ich == 'o')
      drawTextScreen(1);
    else if (ich == 'f')
      fillScreenTest();
    else if (ich == 't')
      drawTextScreen(0);
    else if (ich == 'g')
      drawGFXTextScreen(0);
    else if (ich == 'r') {
      testDMAContUpdate(true);
      Serial.println("Returned from testDMAContUpdate");
    }
    else if (ich == 'a') {
      testDMAContUpdate(false);
      Serial.println("Returned from testDMAContUpdate");
    }
    else
      drawTestScreen();
  }

}

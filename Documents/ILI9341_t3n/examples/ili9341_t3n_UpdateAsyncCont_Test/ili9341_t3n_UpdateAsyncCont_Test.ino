#include <ILI9341_fonts.h>
#include <ILI9341_t3n.h>
#include <ili9341_t3n_font_Arial.h>
#include <ili9341_t3n_font_ArialBold.h>

elapsedMicros _dt;
#define dtSTART {_dt=0;}
#define dtEND(a) { Serial.printf( "\n%s()_%s : dt %ul us", __func__, a, (uint32_t)_dt);}

#define UPDATE_HALF_FRAME

#define ROTATION 3

#include "SPI.h"


//------------------------------------
#define USE_SPI1
#ifdef USE_SPI1
#ifdef ARDUINO_TEENSY41
#define TFT_RST 8
#define TFT_DC  38
#define TFT_CS 37
#define TFT_MOSI 26
#define TFT_MISO 39
#define TFT_SCK 27
#else
#define TFT_RST 8
#define TFT_DC  31
#define TFT_CS 30
#define TFT_MOSI 0
#define TFT_MISO 1
#define TFT_SCK 32

#endif
#elif defined(ARDUINO_TEENSY35) || defined(ARDUINO_TEENSY36)
// currently my flexi...
#define TFT_DC 22
#define TFT_CS 15
#define TFT_RST -1
#define TFT_SCK 14
#define TFT_MISO 12
#define TFT_MOSI 7
#define DEBUG_PIN 13
#else
#define TFT_RST 8
#define TFT_DC  9
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_MISO 12
#define TFT_SCK 13
#endif
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

uint16_t our_pallet[] = {
  ILI9341_BLACK,  ILI9341_RED, ILI9341_GREEN,  ILI9341_BLUE,
  ILI9341_YELLOW, ILI9341_ORANGE, ILI9341_CYAN, ILI9341_PINK
};

#define COUNT_SHUTDOWN_FRAMES 16
volatile uint8_t shutdown_cont_update_count = 0xff;
uint16_t *tft_frame_buffer = nullptr;
#define FRAME_BUFFER_SIZE (320*240*2)
elapsedMicros my_us;


void setup() {
  while (!Serial && (millis() < 4000)) ;
  Serial.begin(115200);
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);
  Serial.printf("Begin: CS:%d, DC:%dRST: %d\n", TFT_CS, TFT_DC, TFT_RST);
  tft.begin(26000000);
  tft.setRotation(ROTATION);
  tft.useFrameBuffer(true);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(ILI9341_t3n::CENTER, ILI9341_t3n::CENTER);
  tft.setTextColor(ILI9341_RED);
  tft.setFont(Arial_20_Bold);
  tft.println("*** Auto start ***");
  tft.updateScreen();
  tft_frame_buffer = tft.getFrameBuffer();
  delay(250);
#ifdef UPDATE_HALF_FRAME
  tft.setFrameCompleteCB(&frame_callback, true);
#else
  tft.setFrameCompleteCB(&frame_callback, false);
#endif
  // We are not running DMA currently so start it up.
  Serial.println("Starting up DMA Updates");
  shutdown_cont_update_count = 0xff;
  tft.updateScreenAsync(true);

  Serial.println("==================================================");

  Serial.println("\n*** Commands ***");
  Serial.println("    t - Toggle display updates on or off");
}

void frame_callback() {
  //Serial.printf("FCB: %d %d\n", tft.frameCount(), tft.subFrameCount());
  uint32_t frameCount = tft.frameCount();
  // See if end of test signalled.
  if (shutdown_cont_update_count == COUNT_SHUTDOWN_FRAMES) {
    uint8_t color_index = (frameCount >> 4) & 0x7;
    tft.setCursor(ILI9341_t3n::CENTER, ILI9341_t3n::CENTER);
    tft.setTextColor(our_pallet[(color_index + 3) & 7]);
    tft.setFont(Arial_20_Bold);
    tft.println("Stop Signalled");
    shutdown_cont_update_count--;
    arm_dcache_flush(tft_frame_buffer, FRAME_BUFFER_SIZE);
  } else if (shutdown_cont_update_count == 0) {
    tft.setCursor(ILI9341_t3n::CENTER, tft.getCursorY());
    tft.println("endUpdateAsync");
    tft.endUpdateAsync();
    Serial.println("after endUpdateAsync");
    arm_dcache_flush(tft_frame_buffer, FRAME_BUFFER_SIZE);
  } else if (shutdown_cont_update_count < COUNT_SHUTDOWN_FRAMES) {
    shutdown_cont_update_count--;
  } else {
#ifdef UPDATE_HALF_FRAME
    bool draw_frame = false;
    if (((frameCount & 0xf) == 0) && tft.subFrameCount()) {
      draw_frame = true;
      tft.setClipRect(0, 0, tft.width(), tft.height() / 2);
    } else if (((frameCount & 0xf) == 1) && !tft.subFrameCount()) {
      draw_frame = true;
      tft.setClipRect(0, tft.height() / 2, tft.width(), tft.height() / 2);
    }
    if (draw_frame)
#else
    if (tft.subFrameCount()) {
      // lets ignore these right now
      return;
    }
    if ((frameCount & 0xf) == 0)
#endif
    {
      // First pass ignore subframe...
      uint8_t color_index = (frameCount >> 4) & 0x7;
      tft.fillScreen(our_pallet[color_index]);
      tft.drawRect(5, 5, tft.width() - 10, tft.height() - 10, our_pallet[(color_index + 1) & 7]);
      tft.drawRect(25, 25, tft.width() - 50, tft.height() - 50, our_pallet[(color_index + 2) & 7]);

      static uint8_t display_other = 0;
      switch (display_other) {
        case 0:
          tft.fillRect(50, 50, tft.width() - 100, tft.height() - 100, our_pallet[(color_index + 1) & 7]);
          break;
        case 1:
          tft.fillCircle(tft.width() / 2, tft.height() / 2, 100, our_pallet[(color_index + 1) & 7]);
          break;
        case 2:
          tft.fillTriangle(50, 50, tft.width() - 50, 50, tft.width() / 2, tft.height() - 50, our_pallet[(color_index + 1) & 7]);
          break;
      }
      if (!tft.subFrameCount()) {
        display_other++;
        if (display_other > 2) display_other =  0 ;
      }

      arm_dcache_flush(tft_frame_buffer, FRAME_BUFFER_SIZE);
      tft.setClipRect();
    }
  }

}

void loop(void) {
  // See if any text entered
  int ich;
  if ((ich = Serial.read()) != -1) {
    while (Serial.read() != -1) ;
    switch (ich) {
      case 't':
        toggleOnOffDisplay();
        break;
    }
  }
}

void toggleOnOffDisplay() {
  if (!tft.asyncUpdateActive()) {
    // We are not running DMA currently so start it up.
    Serial.println("Starting up DMA Updates");
    shutdown_cont_update_count = 0xff;
    tft.updateScreenAsync(true);
  } else {
    shutdown_cont_update_count = COUNT_SHUTDOWN_FRAMES;
    while (shutdown_cont_update_count) ;
    tft.waitUpdateAsyncComplete();
    tft.setCursor(ILI9341_t3n::CENTER, tft.getCursorY());
    tft.print("Finished Test\n");
    Serial.println("after waitUpdateAsyncComplete");
    Serial.println("Finished test");

    delay(2000);
    Serial.println("Do normal update to see if data is there");
    tft.updateScreen();
  }

}

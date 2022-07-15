/********************************************************************
 *
 * ILI9341_T4 library example: Conway's game of life
 *
 ********************************************************************/

#include <Arduino.h>
#include <ILI9341_T4.h>

//
// DEFAULT WIRING USING SPI 0 ON TEENSY 4/4.1
//
#define PIN_SCK 13   // mandatory
#define PIN_MISO 12  // mandatory
#define PIN_MOSI 11  // mandatory
#define PIN_DC \
  20  // mandatory, can be any pin but using pin 10 (or 36 or 37 on T4.1)
      // provides greater performance

#define PIN_CS 15     // optional (but recommended), can be any pin.
#define PIN_RESET 21  // optional (but recommended), can be any pin.
#define PIN_BACKLIGHT \
  22  // optional, set this only if the screen LED pin is connected directly to
      // the Teensy.
#define PIN_TOUCH_IRQ \
  19  // optional, set this only if the touchscreen is connected on the same
      // SPI bus
#define PIN_TOUCH_CS \
  18  // optional, set this only if the touchscreen is connected on the same
      // spi bus

//
// ALTERNATE WIRING USING SPI 1 ON TEENSY 4/4.1
//
//#define PIN_SCK     27      // mandatory
//#define PIN_MISO    1       // mandatory
//#define PIN_MOSI    26      // mandatory
//#define PIN_DC      0       // mandatory, can be any pin but using pin 0 (or
// 38 on T4.1) provides greater performance

//#define PIN_CS      30      // optional (but recommended), can be any pin.
//#define PIN_RESET   29      // optional (but recommended), can be any pin.
//#define PIN_BACKLIGHT 255   // optional, set this only if the screen LED pin
// is connected directly to the Teensy. #define PIN_TOUCH_IRQ 255   // optional,
// set this only if the touchscreen is connected on the same SPI bus #define
// PIN_TOUCH_CS  255   // optional, set this only if the touchscreen is
// connected on the same spi bus

// the screen driver object
ILI9341_T4::ILI9341Driver tft(PIN_CS, PIN_DC, PIN_SCK, PIN_MOSI, PIN_MISO,
                              PIN_RESET, PIN_TOUCH_CS, PIN_TOUCH_IRQ);

// 2 diff buffers with 8K memory each
ILI9341_T4::DiffBuffStatic<1024> diff1;
ILI9341_T4::DiffBuffStatic<1024> diff2;

// Screen size in landscape mode
#define LX 320
#define LY 240

// 30MHz SPI. can do better with short wires
#define SPI_SPEED 30000000

// the framebuffers
DMAMEM uint16_t
    internal_fb[LX * LY];  // used by the library for buffering (in DMAMEM)
PROGMEM uint16_t fb[LX * LY];      // the main framebuffer we draw onto.

// colors
#define BLACK 0
#define WHITE 0xFFFF

// size of a cell in pixels
#define CELL_LX 2  // possible value: 1, 2, 4, 8
#define CELL_LY 2  // possible values: 1, 2, 4, 8

// size of the 'world'
#define WORLD_LX (LX / CELL_LX)
#define WORLD_LY (LY / CELL_LY)

// two world buffers
uint8_t worldA[WORLD_LX * WORLD_LY];
uint8_t worldB[WORLD_LX * WORLD_LY];

// pointers to the 'current' and the 'previous' world
uint8_t* oldworld = worldA;
uint8_t* curworld = worldB;

// swap the two worlds pointers.
void swap_worlds() {
  auto tmp = oldworld;
  oldworld = curworld;
  curworld = tmp;
}

// draw a filled rectangle in the framebuffer
void draw_rect(int x, int y, int lx, int ly, uint16_t color) {
  uint16_t* p = fb + x + (LX * y);
  for (int j = 0; j < ly; j++) {
    for (int i = 0; i < lx; i++) p[i] = color;
    p += LX;
  }
}

// create a random initial world with given density in [0.0f,1.0f]
void random_world(uint8_t* world, float density) {
  memset(world, 0, WORLD_LX * WORLD_LY);
  const int thr = density * 1024;
  for (int j = 1; j < WORLD_LY - 1; j++)
    for (int i = 1; i < WORLD_LX - 1; i++)
      world[i + (j * WORLD_LX)] = (random(0, 1024) < thr) ? 1 : 0;
}

// draw a world on the framebuffer
void draw_world(uint8_t* world, uint16_t color, uint16_t bkcolor) {
  for (int j = 0; j < WORLD_LY; j++)
    for (int i = 0; i < WORLD_LX; i++)
      draw_rect(i * CELL_LX, j * CELL_LY, CELL_LX, CELL_LY,
                world[(j * WORLD_LX) + i] ? color : bkcolor);
}

// number of neighbours of a cell
inline int nb_neighbours(int x, int y, uint8_t* world) {
  const int ind = x + (WORLD_LX * y);
  return world[ind - WORLD_LX - 1] + world[ind - WORLD_LX] +
         world[ind - WORLD_LX + 1] + world[ind - 1] + world[ind + 1] +
         world[ind + WORLD_LX - 1] + world[ind + WORLD_LX] +
         world[ind + WORLD_LX + 1];
}

// compute the world at the next generation.
void compute(uint8_t* cur_world, uint8_t* new_world) {
  for (int j = 1; j < WORLD_LY - 1; j++) {
    for (int i = 1; i < WORLD_LX - 1; i++) {
      const int ind = i + (WORLD_LX * j);
      const int nbn = nb_neighbours(i, j, cur_world);
      if (nbn == 3)
        new_world[ind] = 1;
      else if ((nbn < 2) || (nbn > 3))
        new_world[ind] = 0;
      else
        new_world[ind] = cur_world[ind];
    }
  }
}

void setup_lcd() {
  // Serial.begin(9600);

  tft.output(&Serial);  // output debug infos to serial port.
  while (!tft.begin())
    ;                  // initialize the display
  tft.setRotation(3);  // landscape mode 320x240
  tft.setFramebuffer(
      internal_fb);  // set the internal framebuffer (enables double buffering)
  tft.setDiffBuffers(
      &diff1,
      &diff2);  // set the 2 diff buffers => activate differential updates.
  tft.setDiffGap(4);  // use a small gap for the diff buffers (useful because
                      // cells are small...)
  tft.setRefreshRate(120);  // Set the refresh rate to around 120Hz
  tft.setVSyncSpacing(1);  // set framerate = refreshrate (we must draw the fast
                           // for this to works: ok in our case).

  if (PIN_BACKLIGHT != 255) {  // make sure backlight is on
    pinMode(PIN_BACKLIGHT, OUTPUT);
    digitalWrite(PIN_BACKLIGHT, HIGH);
  }

  random_world(curworld, 0.7f);  // compute initial world ditribution
}

int nbf = 0;  // count the number of frames drawn.

void loop_lcd() {
  compute(curworld, oldworld);         // compute the next geenration
  swap_worlds();                       // swap between old and new worlds
  draw_world(curworld, WHITE, BLACK);  // draw onto the framebuffer

  tft.overlayFPS(
      fb);  // draw the FPS counter on the top right corner of the framebuffer
  tft.update(fb);  // push to screen (asynchronously).

  if (++nbf % 1000 == 500) {  // output some stats on Serial every 1000 frames.
    tft.printStats();         // stats about the driver
    diff1.printStats();       // stats about the first diff buffer
    diff2.printStats();       // stats about the second diff buffer
  }
}

/** end of file */
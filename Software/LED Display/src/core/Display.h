#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include <DMAChannel.h>
#include <stdint.h>

#include "power/Color.h"
#include "power/Math3D.h"

// Choose between WS2812 or PL9823
#define PL9823

class Display {
 public:
  static const uint8_t width = 16;
  static const uint8_t height = 16;
  static const uint8_t depth = 16;
  static const uint8_t gamma8[256];

 private:
  // Amount of bits per led (keep at 24 for RGB)
  static const uint8_t BITCOUNT = 24;
  // Amount of leds per channel
  static const uint16_t LEDCOUNT = 128;
  // DIN (data in, bit to be shifted in on BCK)
  static const uint8_t DIN = 8;
  // WCK (word clock, latches the shiftregisters)
  static const uint8_t WCK = 9;
  // BCK (bit clock, shifts all data bits)
  static const uint8_t BCK = 10;

 private:
  // Hardware setup for PLL, FLEXIO and DMA
  void setupPLL();
  void setupFIO();
  void setupDMA();
  // DMA transfer complete ISR, this flips the dma with the prep buffer
  static void interruptAtCompletion();
  // Instance method for interruptAtCompletion
  void displayReady();
  // Cube data filled with colors. Arranged as x, y, z coordinates
  Color cube[width][height][depth];
  // Buffer currently being used by dma
  volatile uint8_t m_dmaBuffer = 0;
  // Double buffered dma and prep data to prevent display artifacts
  uint32_t dmaBufferData[2][BITCOUNT * LEDCOUNT] = {};
  // Dma buffer for high signal and reset/latch low signal
  uint32_t dmaBufferHigh[1] = {0xFFFFFFFF};
  uint32_t dmaBufferLow[50] = {};
  // See if the display is available to accept a new frame
  volatile uint8_t m_displayAvailable = true;
  // Need 2 dma channels for sending the data to PL9823 LED's
  DMAChannel dmaChannel[2];
  // Need 6 TCD's for sending the data to PL9823 LED's
  DMASetting dmaSetting[6];

 public:
  // Gets a display reference to enable calling public methods
  static Display &instance() {
    // Create exactly one Display object with the private contructor
    static Display display;
    // Return the one and only display object
    return display;
  };
  // Not allowed to make an object of the class (singleton pattern)
  Display(){};
  // Copy constructor is also disabled (singleton pattern)
  Display(Display const &);
  // Assigment operator is also disabled (singleton pattern)
  void operator=(Display const &);

  // Do not use a class contructor to start the display (Arduino compatibility)
  void begin();
  // Notifies the display that a frame is ready for displaying
  void update();
  // Check if the display is available to accept a new frame
  bool available();
  // Clear the cube so a new frame can be created fresh
  void clear();
  // Get a reference to a color from the cube
  Color &color(const uint8_t x, const uint8_t y, const uint8_t z);
  // Radiate light from a pixel in the cube
  void radiate(const Vector3 &, const Color &, float distance);
};
// Ask compiler to inline this so the function call can be skipped
inline Color &Display::color(const uint8_t x, const uint8_t y,
                             const uint8_t z) {
  return cube[x][y][z];
}
#endif
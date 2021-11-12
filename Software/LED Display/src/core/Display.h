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
  static Color cube[width][height][depth];

 private:
  // Gamma correction table
  static const uint8_t gamma8[256];
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
  static void setupPLL();
  static void setupFIO();
  static void setupDMA();
  // DMA transfer complete ISR, this flips the dma with the prep buffer
  static void interruptAtCompletion();
  // Instance method for interruptAtCompletion
  static void displayReady();
  // Buffer currently being used by dma
  static volatile uint8_t dmaBuffer;
  // Double buffered dma and prep data to prevent display artifacts
  static uint32_t dmaBufferData[2][BITCOUNT * LEDCOUNT];
  // Dma buffer for high signal and reset/latch low signal
  static uint32_t dmaBufferHigh[1];
  static uint32_t dmaBufferLow[50];
  // See if the display is available to accept a new frame
  static volatile boolean displayAvailable;
  // Need 2 dma channels for sending the data to PL9823 LED's
  static DMAChannel dmaChannel[2];
  // Need 6 TCD's for sending the data to PL9823 LED's
  static DMASetting dmaSetting[6];

 public:
  // Do not use a class contructor to start the display (Arduino compatibility)
  static void begin();
  // Notifies the display that a frame is ready for displaying
  static void update();
  // Check if the display is available to accept a new frame
  static bool available();
  // Clear the cube so a new frame can be created fresh
  static void clear();
  // Radiate light from a pixel in the cube
  static void radiate(const Vector3 &, const Color &, float distance);
};
#endif
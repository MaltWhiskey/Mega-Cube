#include <Arduino.h>
#include "./Palette.h"

uint16_t rgb_to_uint16(float r, float g, float b)
{
  int ri = (int)(r * 31.0f);
  int gi = (int)(g * 63.0f);
  int bi = (int)(b * 31.0f);
  // Serial.printf("%d %d %d\n", ri, gi, bi);
  return (ri << 11) | (gi << 5) | bi;
}

typedef struct
{
  int i, r, g, b;
} RGB_t;

static RGB_t colors[] = {
    {0, 0, 0, 50},      //dark blue
    {128, 0, 255, 0},   //green
    {164, 255, 220, 0}, //bright yellow
    {192, 255, 128, 0}, //bright orange
    {256, 255, 0, 0}    //full red
};
static int color_count = 5;

Palette::Palette()
{
  int palette_index = 0;
  for (int i = 0; i < color_count - 1; i++)
  {
    RGB_t color1 = colors[i];
    RGB_t color2 = colors[i + 1];
    int range = color2.i - color1.i;
    for (int j = 0; j < range; j++)
    {
      uint8_t r = color1.r + (color2.r - color1.r) * j / range;
      uint8_t g = color1.g + (color2.g - color1.g) * j / range;
      uint8_t b = color1.b + (color2.b - color1.b) * j / range;
      palette[palette_index] = rgb_to_uint16(r / 255.0f, g / 255.0f, b / 255.0f);
      palette_index++;
    }
  }
}

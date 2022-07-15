#include "Arduino.h"
#include "Bitmap.h"

Bitmap::Bitmap(int width, int height)
{
  this->width = width;
  this->height = height;
  pixels = (uint16_t *)malloc(width * height * sizeof(uint16_t));
  memset(pixels, 0, width * height * sizeof(uint16_t));
  rows = (uint16_t **)malloc(height * sizeof(uint16_t **));
  for (int i = 0; i < height; i++)
  {
    rows[i] = pixels + width * i;
  }
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      rows[y][x] = 0;
    }
  }
}

void Bitmap::scroll_left()
{
  for (int y = 0; y < height; y++)
  {
    memmove(rows[y], rows[y] + 1, this->width * sizeof(uint16_t) - 1);
  }
}
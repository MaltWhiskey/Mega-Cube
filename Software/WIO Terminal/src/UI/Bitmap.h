#pragma once
#include <stdint.h>

class Bitmap
{
public:
  uint16_t *pixels;
  uint16_t **rows;
  uint16_t width;
  uint16_t height;
  Bitmap(int width, int height);
  void scroll_left();
};
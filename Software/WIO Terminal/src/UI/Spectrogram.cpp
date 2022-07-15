#include <Arduino.h>
#include "../Display.h"
#include "Spectrogram.h"
#include "Bitmap.h"
#include "Palette.h"

Spectrogram::Spectrogram(Palette *palette, int x, int y, int width, int height) : Component(x, y, width, height)
{
  m_palette = palette;
  this->bitmap = new Bitmap(width, height);
}

void Spectrogram::update(int *mag)
{
  bitmap->scroll_left();
  for (int i = 0; i < bitmap->height; i++)
  {
    bitmap->rows[bitmap->height - i - 1][bitmap->width - 1] = m_palette->get_color(mag[i] / 4);
  }
}

void Spectrogram::_draw(Display &display)
{
  display.setSwapBytes(true);
  display.pushImage(x, y, bitmap->width, bitmap->height, bitmap->pixels);
  display.setSwapBytes(false);
}
#pragma once

#include <stdint.h>

class Palette
{
protected:
  uint16_t palette[256];

public:
  Palette();
  inline uint16_t get_color(int index)
  {
    return palette[max(0, min(255, index))];
  }
};

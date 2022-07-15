#pragma once

#include "Component.h"

class Palette;
class GraphicEqualiser : public Component
{
private:
  Palette *m_palette;
  int m_num_bins;
  float *bar_chart;
  float *bar_chart_peaks;

public:
  GraphicEqualiser(Palette *palette, int x, int y, int width, int height, int num_bins);
  void update(int *mag);
  void _draw(Display &display);
};

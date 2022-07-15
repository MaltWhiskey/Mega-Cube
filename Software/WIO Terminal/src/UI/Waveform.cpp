#include <Arduino.h>
#include "../Display.h"
#include "Waveform.h"

Waveform::Waveform(Display &display, int x, int y, int width, int height, int num_samples) : Component(x, y, width, height)
{
  m_num_samples = num_samples;
  m_samples = static_cast<int *>(malloc(sizeof(int) * num_samples));
}

void Waveform::update(int *samples)
{
  memcpy(m_samples, samples, sizeof(int) * m_num_samples);
}

void Waveform::_draw(Display &display)
{
  float x = 0;
  float x_step = (float)width / (float)m_num_samples;
  display.fillRect(x, y, width, height, 0);
  for (int i = 4; i < m_num_samples; i += 4)
  {
    display.drawLine(x, height / 2 + m_samples[i - 4] / 5, x + x_step * 4, height / 2 + m_samples[i] / 5, 0xfff);
    x += x_step * 4;
  }
}
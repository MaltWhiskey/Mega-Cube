#pragma once

#include <stdint.h>

class Processor
{
private:
public:
  int m_window_size;
  int *m_energy;
  int *m_fft_input;
  int *m_imag;
  int *m_real;

  Processor(int window_size);
  void update(int16_t *samples);
};
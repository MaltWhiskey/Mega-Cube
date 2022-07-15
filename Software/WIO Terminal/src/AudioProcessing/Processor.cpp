#include <Arduino.h>
#include "Processor.h"
#include "approx_fft.h"

Processor::Processor(int window_size)
{
  m_window_size = window_size;
  m_fft_input = static_cast<int *>(malloc(sizeof(int) * window_size));
  m_real = static_cast<int *>(malloc(sizeof(int) * window_size));
  m_imag = static_cast<int *>(malloc(sizeof(int) * window_size));
  m_energy = static_cast<int *>(malloc(sizeof(int) * window_size / 2));
}

void Processor::update(int16_t *samples)
{
  // grab the sample
  for (int i = 0; i < m_window_size; i++)
  {
    m_fft_input[i] = samples[i];
  }
  Approx_FFT(m_fft_input, m_real, m_imag, m_window_size, 8000);

  for (int i = 0; i < m_window_size / 2; i++)
  {
    m_energy[i] = m_real[i];
  }
}
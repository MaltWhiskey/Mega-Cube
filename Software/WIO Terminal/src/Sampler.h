#include <stdint.h>
#pragma once

class Sampler {
 private:
  int sample_rate;
  int buffer_size;
  int16_t *buffer;
  int current_index;

 public:
  Sampler(int sample_rate, int buffer_size);
  void start();
  int16_t *get_buffer() { return buffer; }
  friend void timerIsr();
};
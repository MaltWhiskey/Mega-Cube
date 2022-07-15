#include "Sampler.h"

#include <Arduino.h>

#include "SAMD51_InterruptTimer.h"

static Sampler *instance = NULL;

void timerIsr() {
  instance->buffer[instance->current_index] = analogRead(WIO_MIC);
  instance->current_index++;
  // have we collected enough samples?
  if (instance->current_index >= instance->buffer_size) {
    // just loop back round to the start - doesn't really matter if we overwrite
    // samples
    instance->current_index = 0;
  }
}

Sampler::Sampler(int sample_rate, int buffer_size) {
  this->sample_rate = sample_rate;
  this->buffer_size = buffer_size;
  this->buffer =
      reinterpret_cast<int16_t *>(malloc(buffer_size * sizeof(int16_t)));
  this->current_index = 0;
  pinMode(WIO_MIC, INPUT);
  instance = this;
}

void Sampler::start() { TC.startTimer(1000000 / sample_rate, timerIsr); }

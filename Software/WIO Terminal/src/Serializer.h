#pragma once
#include "TCP.h"

class TCP;

class Serializer {
 private:
  int m_bins;
  TCP *m_tcp;
  boolean btn_a = false;
  boolean btn_b = false;
  boolean btn_c = false;
  boolean btn_stick = false;

 public:
  Serializer(int m_bins, TCP *m_tcp);
  void update(int *fft);
};

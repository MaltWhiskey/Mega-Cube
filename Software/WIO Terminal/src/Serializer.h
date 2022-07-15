#pragma once
#include "TCP.h"

class TCP;

class Serializer {
 private:
  int m_bins;
  TCP *m_tcp;

 public:
  Serializer(int m_bins, TCP *m_tcp);
  void update(int *fft);
};

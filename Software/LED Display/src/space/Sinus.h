#ifndef SINUS_H
#define SINUS_H
#include <stdint.h>

#include "Animation.h"

class Sinus : public Animation {
 private:
  float x1, x2;
  float z1, z2;
  float phase;

 private:
  void init();
  bool draw(float dt);
};
#endif
#ifndef TWINKELS_H
#define TWINKELS_H
#include <stdint.h>

#include "Animation.h"

class Twinkels : public Animation {
 private:
  Timer timer1;
  Timer timer2;
  Timer timer3;
  uint8_t loops = 0;

 private:
  void init();
  bool draw(float dt);
};
#endif
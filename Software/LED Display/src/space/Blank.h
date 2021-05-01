#ifndef BLANK_H
#define BLANK_H
#include <stdint.h>

#include "Animation.h"

class Blank : public Animation {
 private:
 private:
  void init();
  bool draw(float dt);
};
#endif
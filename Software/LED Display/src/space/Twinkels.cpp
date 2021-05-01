
#include "Twinkels.h"

void Twinkels::init() {
  if (loops == 0) loops = random(4, 10);
  timer1 = noise.nextRandom(0.001f, 0.05f);
  timer2 = noise.nextRandom(1.00f, 4.00f);
  timer3 = noise.nextRandom(0.01f, 0.04f);
}

bool Twinkels::draw(float dt) {
  if (timer3.update()) {
    for (uint8_t x = 0; x < display.width; x++)
      for (uint8_t y = 0; y < display.height; y++)
        for (uint8_t z = 0; z < display.depth; z++) {
          Color& c = display.color(x, y, z);
          c.r = 0.93f * c.r;
          c.g = 0.93f * c.g;
          c.b = 0.93f * c.b;
          display.color(x, y, z) = c;
        }
  }
  if (timer1.update()) {
    display.color(random(display.width), random(display.height),
                  random(display.depth)) = Color(0, 255);
  }
  if (timer2.update()) {
    if (--loops == 0)
      return true;
    else
      init();
  }
  return false;
}
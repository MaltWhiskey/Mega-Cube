#ifndef HELIX_H
#define HELIX_H

#include "Animation.h"

class Helix : public Animation {
 private:
  float phase;
  float phase_speed;
  int16_t hue_speed;

  uint8_t bottom;
  uint8_t top;
  uint8_t thickness;
  uint8_t stage;
  Timer timer_duration;
  Timer timer_interval;

 public:
  void init() {
    task = task_state_t::RUNNING;
    bottom = 0;
    top = 0;
    thickness = 1;
    stage = 0;
    timer_duration = 20.0f;
    timer_interval = 0.2f;
    phase_speed = 5;
    hue_speed = 30;
  }

  void draw(float dt) {
    phase += phase_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);

    for (int y = bottom; y < top; y++) {
      for (int i = 0; i < thickness; i++) {
        float xf = sinf(phase + mapf(y, 0, Display::height - 1, 0, 2 * PI) +
                        i * PI / 60);
        float zf = cosf(phase + mapf(y, 0, Display::height - 1, 0, 2 * PI) +
                        i * PI / 60);
        xf = mapf(xf, -1.1f, 0.9f, 0, Display::width - 1);
        zf = mapf(zf, -1.1f, 0.9f, 0, Display::height - 1);
        Color c1 = Color((hue16 >> 8) + y * 3, RainbowGradientPalette);
        Color c2 = Color((hue16 >> 8) + y * 3 + 128, RainbowGradientPalette);
        Vector3 v1 = Vector3(xf, y, zf);
        Vector3 v2 = Vector3(Display::width - xf, y, Display::depth - zf);
        Display::radiate(v1, c1, 1.0f);
        Display::radiate(v2, c2, 1.0f);
      }
    }

    if (timer_interval.update()) {
      int state = 0;
      if (stage == state++) top < Display::height ? top++ : stage++;
      if (stage == state++) thickness < 20 ? thickness++ : stage++;
      if (stage == state++)
        if (timer_duration.update()) stage++;
      if (stage == state++) bottom < Display::height ? bottom++ : stage++;
      if (stage == state++) task = task_state_t::INACTIVE;
    }
  }
};
#endif
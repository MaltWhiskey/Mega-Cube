#ifndef ARROWS_H
#define ARROWS_H

#include "Animation.h"
#include "Power/Math8.h"

class Arrows : public Animation {
 private:
  float angle;
  float angular_speed;
  uint16_t hue_speed;

  Timer timer_duration;
  Timer timer_interval;
  float time_expansion = 2.0f;
  float time_contraction = 5.0f;

  const uint16_t bitmap[16] = {
      0b0000000110000000,  // 0
      0b0000001111000000,  // 1
      0b0000011111100000,  // 2
      0b0000111111110000,  // 3
      0b0001111111111000,  // 4
      0b0011111111111100,  // 5
      0b0000001111000000,  // 6
      0b0000001111000000,  // 7
      0b0000001111000000,  // 8
      0b0000001111000000,  // 9
  };

 public:
  void init() {
    task = task_state_t::RUNNING;
    timer_duration = 60.0f;

    angle = 0;
    angular_speed = -200.0f;
    hue_speed = 50;
  }

  void draw(float dt) {
    angle += angular_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);
    Quaternion axes[3] = {Quaternion(angle * 1.0f, Vector3(1, 0, 0)),
                          Quaternion(angle * 0.9f, Vector3(0, 1, 0)),
                          Quaternion(angle * 0.8f, Vector3(0, 0, 1))};
    Quaternion r = axes[1] * Quaternion(90, Vector3(0, 0, 1));
    for (uint8_t y = 0; y < 10; y++) {
      uint16_t mask = 0x8000;
      for (uint8_t x = 0; x < 16; x++) {
        if (bitmap[y] & mask) {
          // Color c = Color((hue16 >> 8), RainbowGradientPalette);
          Vector3 v, p;
          v = Vector3(x - 7.5f, y - 5.0f, 0 - 6.0f);
          p = axes[0].rotate(v) + Vector3(7.5f, 7.5f, 7.5f);
          Display::radiate(p, Color::RED, 1.8f, true);
          p = r.rotate(v) + Vector3(7.5f, 7.5f, 7.5f);
          Display::radiate(p, Color::GREEN, 1.8f, true);
          p = axes[2].rotate(v) + Vector3(7.5f, 7.5f, 6.0f + 7.5f);
          Display::radiate(p, Color::BLUE, 1.8f, true);
        }
        mask >>= 1;
      }
    }
    // Display::line(axis.v);

    if (timer_duration.update()) {
      task = task_state_t::INACTIVE;
    }
  }
};
#endif
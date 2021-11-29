#ifndef MARIO_H
#define MARIO_H

#include "Animation.h"
#include "gfx/Mario16x16.h"
#include "power/Math8.h"

class Mario : public Animation {
 private:
  float angle;
  float angular_speed;
  uint16_t hue_speed;

  Timer timer_duration;
  Timer timer_interval = 0.15f;
  uint8_t frame = 0;
  uint8_t frame_display[6] = {0, 1, 2, 3, 2, 1};

 public:
  void init() {
    task = task_state_t::RUNNING;
    timer_duration = 60.0f;

    angle = 0;
    angular_speed = -80.0f;
    hue_speed = 10;
  }

  void draw(float dt) {
    angle += angular_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);
    Quaternion axes[3] = {Quaternion(angle * 1.0f, Vector3(1, 0, 0)),
                          Quaternion(angle * 0.9f, Vector3(0, 1, 0)),
                          Quaternion(angle * 0.8f, Vector3(0, 0, 1))};
    // Quaternion r = axes[1] * Quaternion(90, Vector3(0, 0, 1));
    for (uint8_t y = 0; y < 16; y++) {
      for (uint8_t x = 0; x < 16; x++) {
        uint32_t data = mario16x16_data[frame_display[frame]][y * 16 + x];
        Color c = Color(0, 0, 0);
        if (data >> 24 & 0xff) {
          c = Color(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
          if (c.isBlack())
            c = Color(0, 0, 30);
          else {
            c.scale(200);
            c = Color(Display::gamma8[c.r], Display::gamma8[c.g],
                      Display::gamma8[c.b]);
          }
        }
        Vector3 v, p;
        v = Vector3(x - 7.5f, 15 - y - 7.5f, 0 - 7.5f);
        // p = axes[0].rotate(v) + Vector3(7.5f, 7.5f, 7.5f);
        // Display::radiate(p, c, 1.8f, true);
        p = axes[1].rotate(v) * 1.0f + Vector3(7.5f, 7.5f, 7.5f);
        Display::radiate(p, c, 1.5f, false);
        // p = axes[2].rotate(v) + Vector3(7.5f, 7.5f, 6.0f + 7.5f);
        // Display::radiate(p, c, 1.8f, true);
        // Display::cube[x][15 - y][hue16 >> 12] = c;
        // Display::radiate(Vector3(x, 15 - y, (float)hue16 / 4096.0f), c, 1.0f,
        //                 false);
      }
    }
    // Display::line(axis.v);
    if (timer_duration.update()) {
      task = task_state_t::INACTIVE;
    }
    if (timer_interval.update()) {
      if (++frame >= sizeof(frame_display)) frame = 0;
    }
  }
};
#endif
#ifndef STARFIELD_H
#define STARFIELD_H

#include "Animation.h"

class Starfield : public Animation {
 private:
  float phase;
  float phase_speed;
  int16_t hue_speed;

  Timer timer;
  float time_duration;
  float time_fade_out;

  static const int numStars = 100;
  Vector3 stars[numStars];
  bool initialized = false;

 public:
  void init(float fade_in, float duration, float fade_out) {
    if (!initialized) {
      for (int i = 0; i < numStars; i++) {
        stars[i].x = random(0, Display::width);
        stars[i].y = random(0, Display::height);
        stars[i].z = random(0, Display::depth);
      }
      initialized = true;
    }
    phase = 0.0f;
    task = task_state_t::STARTING;
    timer = fade_in;
    time_duration = duration;
    time_fade_out = fade_out;
  }

  void speed(float phase_speed_, int16_t hue_speed_) {
    phase_speed = phase_speed_;
    hue_speed = hue_speed_;
  }

  void draw(float dt) {
    phase += phase_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);

    uint8_t scale = 0;
    if (task == task_state_t::ENDING) {
      if (timer.update()) {
        task = task_state_t::INACTIVE;
        scale = 0;
      } else
        scale = (1 - timer.percent()) * 255;
    }

    if (task == task_state_t::RUNNING) {
      if (timer.update()) {
        task = task_state_t::ENDING;
        timer = time_fade_out;
      }
      scale = 255;
    }

    if (task == task_state_t::STARTING) {
      if (timer.update()) {
        task = task_state_t::RUNNING;
        timer = time_duration;
        scale = 255;
      } else
        scale = timer.percent() * 255;
    }

    for (int i = 0; i < numStars; i++) {
      float sx = (stars[i].x - (Display::width / 2)) / 1.5f;
      float sy = (stars[i].y - (Display::height / 2)) / 1.5f;
      float sz = (stars[i].z) / 1.5f;
      float h = 0.8f + sqrt(sx * sx + sy * sy + sz * sz);
      stars[i].z += sinf(phase) * dt * h * 15;
      if (stars[i].z >= Display::depth) {
        stars[i].x = random(0, Display::width);
        stars[i].y = random(0, Display::height);
        stars[i].z = 0;
      } else if (stars[i].z <= 0) {
        stars[i].x = random(0, Display::width);
        stars[i].y = random(0, Display::height);
        stars[i].z = Display::depth - 1;
      }
      Color c = Color((hue16 >> 8) + stars[i].z, RainbowGradientPalette);
      Display::radiate(Vector3(stars[i].x, stars[i].y, stars[i].z),
                       c.scale(scale), 1.0f);
    }
  }
};
#endif
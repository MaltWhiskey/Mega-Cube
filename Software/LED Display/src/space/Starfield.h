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

  static const int numStars = 200;
  Vector3 stars[numStars];
  bool initialized = false;

 public:
  void init(float fade_in, float duration, float fade_out) {
    if (!initialized) {
      for (int i = 0; i < numStars; i++) {
        stars[i] = Vector3(noise.nextRandom(-1, 1), noise.nextRandom(-1, 1),
                           noise.nextRandom(-1, 1));
      }
      initialized = true;
    }
    phase = 0.0f;
    state = state_t::STARTING;
    timer = fade_in;
    time_duration = duration;
    time_fade_out = fade_out;
  }

  void speed(float phase_speed_, int16_t hue_speed_) {
    phase_speed = phase_speed_;
    hue_speed = hue_speed_;
  }

  void end() {
    if (state == state_t::RUNNING) {
      state = state_t::ENDING;
      timer = time_fade_out;
    }
  }

  void draw(float dt) {
    setMotionBlur(config.animation.starfield.motionBlur);
    phase += phase_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);

    uint8_t scale = 0;
    if (state == state_t::ENDING) {
      if (timer.update()) {
        state = state_t::INACTIVE;
        scale = 0;
      } else
        scale = (1 - timer.percent()) * 255;
    }
    if (state == state_t::RUNNING) {
      if (timer.update()) {
        state = state_t::ENDING;
        timer = time_fade_out;
      }
      scale = 255;
    }
    if (state == state_t::STARTING) {
      if (timer.update()) {
        state = state_t::RUNNING;
        timer = time_duration;
        scale = 255;
      } else
        scale = timer.percent() * 255;
    }

    Quaternion q = Quaternion(000.0f * phase, Vector3(0, 1, 0));
    for (int i = 0; i < numStars; i++) {
      float r = (stars[i] * 3 - Vector3(0, 0, -2.0f)).magnitude();
      stars[i].z += sinf(phase) * 1.75f * dt * r;
      if (stars[i].z > 1) {
        stars[i] =
            Vector3(noise.nextRandom(-1, 1), noise.nextRandom(-1, 1), -1);
      } else if (stars[i].z < -1) {
        stars[i] = Vector3(noise.nextRandom(-1, 1), noise.nextRandom(-1, 1), 1);
      }
      Color c = Color((hue16 >> 8) + r * 6, RainbowGradientPalette);
      // Multiply by sqrt(3) * radius = 12.99 => 13
      voxel(q.rotate(stars[i]) * 13.0f, c.scale(scale));
    }
  }
};
#endif
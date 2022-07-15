#ifndef SINUS_H
#define SINUS_H

#include "Animation.h"

class Sinus : public Animation {
 private:
  float x_min = -2;
  float x_max = 2;
  float z_min = -2;
  float z_max = 2;

  float phase = 0.0f;
  float phase_speed = 0.0f;
  int16_t hue_speed = 0;

  Timer timer;
  float time_duration;
  float time_fade_out;
  float radius;

 public:
  void init(float fade_in, float duration, float fade_out, float radius_) {
    state = state_t::STARTING;
    timer = fade_in;

    phase = 0.0f;
    time_duration = duration;
    time_fade_out = fade_out;
    radius = radius_;
  }

  void speed(float phase_speed_, int16_t hue_speed_) {
    phase_speed = phase_speed_;
    hue_speed = hue_speed_;
  }

  void end() {
    if (state == state_t::RUNNING) {
      state = state_t::ENDING;
      timer = 2.0f;
    }
  }

  void draw(float dt) {
    setMotionBlur(config.animation.sinus.motionBlur);
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

    // Resize the function to be big enough to have the rotated version fit
    // sqrt(3) * 7.5 * 2 => 26 is big enough but more resolution is better
    const float size = 30;
    Quaternion q = Quaternion(phase * 10.0f, Vector3(1, 1, 1));
    for (uint16_t x = 0; x <= size; x++) {
      // convert cube x to floating point coordinate between x_min and x_max
      float xprime = mapf(x, 0, size, x_min, x_max);
      for (uint16_t z = 0; z <= size; z++) {
        // convert cube z to floating point coordinate between z_min and z_max
        float zprime = mapf(z, 0, size, z_min, z_max);
        // determine y floating point coordinate
        float y = sinf(phase + sqrtf(xprime * xprime + zprime * zprime));
        // display voxel on the cube scaled back to radius fitting the cube
        Vector3 point = Vector3(2 * (x / size) - 1, 2 * (z / size) - 1, y);
        point = q.rotate(point) * radius;
        Color c = Color((hue16 >> 8) + ((y + 1) * 64), RainbowGradientPalette);
        radiate(point, c.scale(scale), 1.0f);
      }
    }
  }
};
#endif
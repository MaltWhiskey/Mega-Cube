#ifndef SINUS_H
#define SINUS_H

#include "Animation.h"

class Sinus : public Animation {
 private:
  float x_min = -2;
  float x_max = 2;
  float z_min = -2;
  float z_max = 2;

  float radius;
  float resolution;
  float phase;
  float phase_speed;

  static constexpr auto &settings = config.animation.sinus;

 public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    resolution = settings.resolution;
    hue16_speed = settings.hue_speed * 255;
    radius = settings.radius;

    phase = 0;
    phase_speed = settings.phase_speed;
    setMotionBlur(settings.motionBlur);
  }

  void draw(float dt) {
    uint8_t brightness = settings.brightness;
    phase += dt * phase_speed;
    hue16 += dt * hue16_speed;

    if (state == state_t::STARTING) {
      if (timer_starting.update()) {
        state = state_t::RUNNING;
        timer_running.reset();
      } else {
        brightness *= timer_starting.ratio();
      }
    }
    if (state == state_t::RUNNING) {
      if (timer_running.update()) {
        state = state_t::ENDING;
        timer_ending.reset();
      }
    }
    if (state == state_t::ENDING) {
      if (timer_ending.update()) {
        state = state_t::INACTIVE;
        brightness = 0;
      } else {
        brightness *= (1 - timer_ending.ratio());
      }
    }

    // Resize the function to be big enough to have the rotated version fit
    // sqrt(3) * 7.5 * 2 => 26 is big enough but more resolution is better
    Quaternion q = Quaternion(phase * 10.0f, Vector3(1, 1, 1));
    for (uint16_t x = 0; x <= resolution; x++) {
      // convert cube x to floating point coordinate between x_min and x_max
      float xprime = mapf(x, 0, resolution, x_min, x_max);
      for (uint16_t z = 0; z <= resolution; z++) {
        // convert cube z to floating point coordinate between z_min and z_max
        float zprime = mapf(z, 0, resolution, z_min, z_max);
        // determine y floating point coordinate
        float y = sinf(phase + sqrtf(xprime * xprime + zprime * zprime));
        // display voxel on the cube scaled back to radius fitting the cube
        Vector3 point =
            Vector3(2 * (x / resolution) - 1, 2 * (z / resolution) - 1, y);
        point = q.rotate(point) * radius;
        Color c =
            Color((hue16 >> 8) + (int8_t)(y * 64), RainbowGradientPalette);
        radiate(point, c.scale(brightness), 1.0f);
      }
    }
  }
};
#endif
#ifndef ARROWS_H
#define ARROWS_H

#include "Animation.h"
#include "power/Math8.h"

class Arrows : public Animation {
private:
  float angle;
  float angle_speed;

  float distance;
  float radius_start;
  float radius_max;
  float arc;

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

  static constexpr auto &settings = config.animation.arrows;

public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    angle = 0;
  }

  void draw(float dt) {
    angle_speed = settings.angle_speed;
    hue16_speed = settings.hue_speed * 255;
    radius_max = settings.radius;
    radius_start = settings.radius_start;
    distance = settings.distance;
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();
    float radius = radius_max;

    if (state == state_t::STARTING) {
      if (timer_starting.update()) {
        state = state_t::RUNNING;
        timer_running.reset();
      }
      else {
        brightness *= timer_starting.ratio();
        radius *= timer_starting.ratio();
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
      }
      else {
        brightness *= (1 - timer_ending.ratio());
        radius *= (1 - timer_ending.ratio());
      }
    }
    if (radius < radius_start) {
      radius = radius_start;
    }

    angle += dt * angle_speed;
    hue16 += dt * hue16_speed;
    arc = 2 * (180 / M_PI) * asinf(0.5f / radius);

    // Rotation arround arbitrary angle and axis to add some chaos
    Quaternion q2 = Quaternion(angle * 0.6f, Vector3(1, 1, 1));
    Quaternion q3 = Quaternion(angle * 0.7f, Vector3(-1, -1, -1));
    Quaternion q4 = Quaternion(angle * 0.8f, Vector3(0, 1, 0));

    for (uint8_t y = 0; y < 10; y++) {
      uint16_t mask = 0x8000;
      for (uint8_t x = 0; x < 16; x++) {
        if (bitmap[y] & mask) {
          // Map to coordinates with center (0,0,0) scaled by radius
          Vector3 point = Vector3(x - 7.5f, 4.5f - y, 0) / 7.5f * radius;
          // Project to a line at the bottom of the coordinate system
          Vector3 line = Vector3(point.x, -radius, 0);
          // Rotation around X-axis with increased rotation by y offset
          Quaternion q1 = Quaternion(angle - (arc * point.y), Vector3::X);
          // Apply both rotations on the projected line and fill in color
          Color c = Color((hue16 >> 8) + 000 + 8 * y, RainbowGradientPalette);
          radiate((q2 * q1).rotate(line * 0.8f), c.scale(brightness), distance);
          c = Color((hue16 >> 8) + 064 + 8 * y, RainbowGradientPalette);
          radiate((q3 * q1).rotate(line * 0.9f), c.scale(brightness), distance);
          c = Color((hue16 >> 8) + 128 + 8 * y, RainbowGradientPalette);
          radiate((q4 * q1).rotate(line * 1.0f), c.scale(brightness), distance);
        }
        mask >>= 1;
      }
    }
  }
};
#endif
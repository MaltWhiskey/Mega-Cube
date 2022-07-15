#ifndef ARROWS_H
#define ARROWS_H

#include "Animation.h"
#include "Power/Math8.h"

class Arrows : public Animation {
 private:
  float angle;
  float angular_speed;
  uint16_t hue_speed;
  float max_radius;
  float start_radius = 2;
  float start_angle = 0;
  float radius;
  float arc;

  Timer timer_duration;
  float time_expansion;
  float time_contraction;

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
  void init(float duration, float expansion, float contraction,
            float anglespeed, float radius_, uint16_t hue_speed_) {
    state = state_t::STARTING;
    timer_duration = duration;
    angular_speed = anglespeed;
    hue_speed = hue_speed_;
    max_radius = radius_;
    time_expansion = expansion;
    time_contraction = contraction;
    angle = start_angle;
    radius = start_radius;
  }

  void draw(float dt) {
    setMotionBlur(config.animation.arrows.motionBlur);
    if (timer_duration.update()) {
      state = state_t::ENDING;
    }
    if (state == state_t::STARTING) {
      radius += dt * max_radius / time_expansion;
      if (radius > max_radius) {
        radius = max_radius;
        state = state_t::RUNNING;
      }
    }
    if (state == state_t::ENDING) {
      radius -= dt * max_radius / time_contraction;
      if (radius < start_radius) {
        radius = start_radius;
        state = state_t::INACTIVE;
      }
    }
    uint8_t scale = 200;
    float threshold = 0.4f;
    float progress = (radius - start_radius) / max_radius;
    if (progress < threshold) {
      scale *= (progress * (1 / threshold));
    }

    angle += angular_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);
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
          radiate((q2 * q1).rotate(line * 0.8f), c.scale(scale), 1.5);
          c = Color((hue16 >> 8) + 064 + 8 * y, RainbowGradientPalette);
          radiate((q3 * q1).rotate(line * 0.9f), c.scale(scale), 1.5);
          c = Color((hue16 >> 8) + 128 + 8 * y, RainbowGradientPalette);
          radiate((q4 * q1).rotate(line * 1.0f), c.scale(scale), 1.5);
        }
        mask >>= 1;
      }
    }
  }
};
#endif
#ifndef MARIO_H
#define MARIO_H

#include "Animation.h"
#include "gfx/Mario.h"
#include "power/Math8.h"

class Mario : public Animation {
 private:
  float angle;
  float angular_speed;
  float max_radius;
  float start_radius = 3;
  float start_angle = 0;
  float radius;
  float arc;

  Timer timer_duration;
  Timer timer_interval;
  float time_expansion;
  float time_contraction;

  uint8_t frame = 0;
  uint8_t frame_display[6] = {0, 1, 2, 3, 2, 1};

 public:
  void init(float duration, float expansion, float contraction, float interval,
            float anglespeed, float radius_) {
    state = state_t::STARTING;
    timer_duration = duration;
    timer_interval = interval;
    angular_speed = anglespeed;
    max_radius = radius_;
    time_expansion = expansion;
    time_contraction = contraction;
    angle = start_angle;
    radius = start_radius;
  }

  void draw(float dt) {
    setMotionBlur(config.animation.mario.motionBlur);
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
    if (timer_interval.update()) {
      if (++frame >= sizeof(frame_display)) frame = 0;
    }

    uint8_t scale = 100;
    float threshold = 0.4f;
    float progress = (radius - start_radius) / max_radius;
    if (progress < threshold) {
      scale *= (progress * (1 / threshold));
    }

    angle += angular_speed * dt;
    arc = 2 * (180 / M_PI) * asinf(0.5f / radius);

    for (uint8_t y = 0; y < 16; y++) {
      for (uint8_t x = 0; x < 16; x++) {
        uint32_t data = mario_data[frame_display[frame]][y * 16 + x];
        Color c = Color(0, 0, 0);
        if (data >> 24 & 0xff) {
          c = Color(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
          if (c.isBlack())
            c = Color(0, 0, 0);
          else {
            c.gamma().scale(scale);
          }
        }
        // Map to coordinates with center (0,0,0) scaled by radius
        // Project on a line at the left of the coordinate system
        Vector3 point = Vector3(-radius, 7.5f - y, 0) / 7.5f * radius;
        Quaternion q = Quaternion(angle - (arc * x), Vector3::Y);
        radiate(q.rotate(point), c, 1.0f);
      }
    }
  }
};
#endif
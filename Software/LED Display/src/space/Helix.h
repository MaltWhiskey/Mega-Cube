#ifndef HELIX_H
#define HELIX_H

#include "Animation.h"

class Helix : public Animation {
 private:
  float radius;
  float resolution;

  float angle;
  float angle_speed;
  float phase;
  float phase_speed;

  uint8_t bottom;
  uint8_t top;
  uint8_t thickness;
  uint8_t stage;

  Timer timer_interval;

  static constexpr auto &settings = config.animation.helix;

 public:
  void init() {
    state = state_t::RUNNING;
    timer_running = settings.runtime;
    timer_interval = settings.interval;
    phase_speed = settings.phase_speed;
    angle = settings.angle;
    angle_speed = settings.angle_speed;
    hue16_speed = settings.hue_speed * 255;
    radius = settings.radius;
    resolution = settings.resolution;

    phase = 0;
    bottom = 0;
    top = 0;
    thickness = 0;
    stage = 0;
  }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness;

    phase += dt * phase_speed;
    angle += dt * angle_speed;
    hue16 += dt * hue16_speed;

    Quaternion q1 = Quaternion(180, Vector3(0, 1, 0));
    Quaternion q2 = Quaternion(angle, Vector3(1, 0, 0));

    // Resize the function to be big enough to have the rotated version fit
    // sqrt(3) * 7.5 * 2 => 26 is big enough but more resolution is better
    for (uint16_t y = bottom; y <= top; y++) {
      float xf = sinf(phase + mapf(y, 0, resolution, 0, 2 * PI));
      float zf = cosf(phase + mapf(y, 0, resolution, 0, 2 * PI));
      Vector3 p0 = Vector3(xf, 2 * (y / resolution) - 1, zf) * radius;
      Vector3 p1 = q2.rotate(p0);
      Vector3 p2 = (q2 * q1).rotate(p0);
      Color c1 = Color((hue16 >> 8) + y * 2 + 000, RainbowGradientPalette);
      Color c2 = Color((hue16 >> 8) + y * 2 + 128, RainbowGradientPalette);
      radiate(p1, c1.scale(brightness), 1.0f + (float)thickness / 20.0f);
      radiate(p2, c2.scale(brightness), 1.0f + (float)thickness / 20.0f);
    }
    if (timer_interval.update()) {
      int progress = 0;
      if (stage == progress++) top <= resolution ? top++ : stage++;
      if (stage == progress++)
        thickness <= settings.thickness ? thickness++ : stage++;
      if (stage == progress++)
        if (timer_running.update() || (state == state_t::ENDING)) stage++;
      if (stage == progress++) bottom <= resolution ? bottom++ : stage++;
      if (stage == progress++) state = state_t::INACTIVE;
    }
  }
  void end() {
    if (state == state_t::RUNNING) {
      timer_interval = timer_interval.set_time() / 5;
      state = state_t::ENDING;
      time_reduction = true;
    } else if (state == state_t::ENDING && !time_reduction) {
      timer_interval = timer_interval.set_time() / 5;
      state = state_t::ENDING;
      time_reduction = true;
    }
  }
};
#endif
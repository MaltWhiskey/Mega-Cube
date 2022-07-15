#ifndef HELIX_H
#define HELIX_H

#include "Animation.h"

class Helix : public Animation {
 private:
  float phase;
  float phase_speed;
  int16_t hue_speed;

  uint8_t bottom;
  uint8_t top;
  uint8_t thickness;
  uint8_t stage;
  Timer timer_duration;
  Timer timer_interval;

 public:
  void init(float duration, float interval) {
    state = state_t::RUNNING;
    timer_duration = duration;
    timer_interval = interval;

    phase = 0;
    bottom = 0;
    top = 0;
    thickness = 1;
    stage = 0;
  }

  void speed(float phase_speed_, int16_t hue_speed_) {
    phase_speed = phase_speed_;
    hue_speed = hue_speed_;
  }

  void draw(float dt) {
    setMotionBlur(config.animation.helix.motionBlur);
    phase += phase_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);

    // Resize the function to be big enough to have the rotated version fit
    // sqrt(3) * 7.5 * 2 => 26 is big enough but more resolution is better
    const float size = 30;
    // TODO get radius from configuration
    const float radius = 7.5f;
    Quaternion q1 = Quaternion(180, Vector3(0, 1, 0));
    // TODO get rotation speed and axis from configuration
    Quaternion q2 = Quaternion(0 * 5 * phase, Vector3(1, 0, 0));

    for (uint16_t y = bottom; y <= top; y++) {
      float xf = sinf(phase + mapf(y, 0, size, 0, 2 * PI));
      float zf = cosf(phase + mapf(y, 0, size, 0, 2 * PI));
      Vector3 p0 = Vector3(xf, 2 * (y / size) - 1, zf) * radius;
      Vector3 p1 = q2.rotate(p0);
      Vector3 p2 = (q2 * q1).rotate(p0);
      Color c1 = Color((hue16 >> 8) + y * 2 + 000, RainbowGradientPalette);
      Color c2 = Color((hue16 >> 8) + y * 2 + 128, RainbowGradientPalette);
      radiate(p1, c1, 1.0f + (float)thickness / 20.0f);
      radiate(p2, c2, 1.0f + (float)thickness / 20.0f);
    }
    if (timer_interval.update()) {
      int progress = 0;
      if (stage == progress++) top <= size ? top++ : stage++;
      if (stage == progress++) thickness <= 16 ? thickness++ : stage++;
      if (stage == progress++)
        if (timer_duration.update() || (state == state_t::ENDING)) stage++;
      if (stage == progress++) bottom <= size ? bottom++ : stage++;
      if (stage == progress++) state = state_t::INACTIVE;
    }
  }
};
#endif
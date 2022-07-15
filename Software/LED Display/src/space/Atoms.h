#ifndef ATOMS_H
#define ATOMS_H

#include "Animation.h"
#include "Power/Math8.h"

class Atoms : public Animation {
 private:
  const static uint8_t ATOMS = 9;

  float angle;
  float angular_speed;
  uint16_t hue_speed;
  float max_radius;
  float start_radius = 0;
  float start_angle = 0;
  float radius;

  Timer timer_duration;
  float time_expansion;
  float time_contraction;

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
    setMotionBlur(config.animation.atoms.motionBlur);
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
    uint8_t scale = 255;
    float threshold = 0.4f;
    float progress = (radius - start_radius) / max_radius;
    if (progress < threshold) {
      scale *= (progress * (1 / threshold));
    }

    angle += angular_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);
    float a = angle * 1.0f;
    float t = angle * 0.1f;

    Quaternion axes[ATOMS] = {
        Quaternion(t, Vector3(+sinf(a / 95), +sinf(a / 75), -sinf(a / 95))),
        Quaternion(t, Vector3(+sinf(a / 90), -sinf(a / 85), -sinf(a / 95))),
        Quaternion(t, Vector3(-sinf(a / 94), +sinf(a / 80), -sinf(a / 75))),
        Quaternion(t, Vector3(+sinf(a / 90), +sinf(a / 70), -sinf(a / 90))),
        Quaternion(t, Vector3(+sinf(a / 80), -sinf(a / 70), -sinf(a / 99))),
        Quaternion(t, Vector3(-sinf(a / 99), +sinf(a / 90), -sinf(a / 80))),
        Quaternion(t, Vector3(-sinf(a / 90), -sinf(a / 90), +sinf(a / 99))),
        Quaternion(t, Vector3(-sinf(a / 70), -sinf(a / 80), -sinf(a / 90))),
        Quaternion(t, Vector3(-sinf(a / 99), +sinf(a / 70), +sinf(a / 80)))};

    // Use normalized vectors to limit radius to length 1
    Vector3 atoms[ATOMS] = {Vector3(1, 0, 0),
                            Vector3(0, 1, 0),
                            Vector3(0, 0, 1),
                            Vector3(-1, 0, 0),
                            Vector3(0, -1, 0),
                            Vector3(0, 0, -1),
                            Vector3(1, 0, 1).normalize(),
                            Vector3(1, 1, 0).normalize(),
                            Vector3(0, 1, 1).normalize()};

    for (uint8_t i = 0; i < ATOMS; i++) {
      Vector3 v = axes[i].rotate(atoms[i]) * radius;
      Color c = Color((hue16 >> 8) + (i * 8), RainbowGradientPalette);
      radiate4(v, c.scale(scale), 4.0f);
      // line(axes[i].v);
    }
  }
};
#endif
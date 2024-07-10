#ifndef ATOMS_H
#define ATOMS_H

#include "Animation.h"
#include "power/Math8.h"

class Atoms : public Animation {
private:
  float angle;
  float angle_speed;

  float distance;
  float radius_start;
  float radius_max;

  static constexpr auto &settings = config.animation.atoms;

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

    float a = angle * 1.0f;
    float t = angle * 0.1f;

    Quaternion axes[] = {
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
    Vector3 atoms[] = {Vector3(1, 0, 0),
                       Vector3(0, 1, 0),
                       Vector3(0, 0, 1),
                       Vector3(-1, 0, 0),
                       Vector3(0, -1, 0),
                       Vector3(0, 0, -1),
                       Vector3(1, 0, 1).normalize(),
                       Vector3(1, 1, 0).normalize(),
                       Vector3(0, 1, 1).normalize()};

    uint8_t ATOMS = sizeof(atoms) / sizeof(Vector3);
    for (uint8_t i = 0; i < ATOMS; i++) {
      Vector3 v = axes[i].rotate(atoms[i]) * radius;
      Color c = Color((hue16 >> 8) + (i * 8), RainbowGradientPalette);
      radiate5(v, c.scale(brightness), distance);
    }
  }
};
#endif
#ifndef ATOMS_H
#define ATOMS_H

#include "Animation.h"
#include "Power/Math8.h"

class Atoms : public Animation {
 private:
  float angle1;
  float angular_speed1;
  float angle2;
  float angular_speed2;
  int16_t hue_speed;
  bool expansion;

  Timer timer_duration;
  Timer timer_interval;
  float time_expansion = 2.0f;
  float time_contraction = 5.0f;

  const static uint8_t ATOMS = 6;
  Vector3 atoms[ATOMS] = {
      Vector3(1, 0, 0),  Vector3(0, 1, 0),  Vector3(0, 0, 1),
      Vector3(-1, 0, 0), Vector3(0, -1, 0), Vector3(0, 0, -1),
  };

 public:
  void init() {
    task = task_state_t::RUNNING;
    timer_duration = 60.0f;
    timer_interval = time_expansion;

    angle1 = 0;
    angle2 = 0;
    expansion = true;
    angular_speed1 = 55.0f;
    angular_speed2 = 73.0f;
    hue_speed = 30;
  }

  void draw(float dt) {
    angle1 += angular_speed1 * dt;
    angle2 -= angular_speed2 * dt;

    hue16 += (int16_t)(255 * hue_speed * dt);

    Vector3 axes[ATOMS];
    axes[0] =
        Vector3(+sinf(angle1 / 90), +sinf(angle1 / 70), -sinf(angle1 / 90));
    axes[1] =
        Vector3(+sinf(angle1 / 80), -sinf(angle1 / 70), -sinf(angle1 / 99));
    axes[2] =
        Vector3(-sinf(angle1 / 99), +sinf(angle1 / 90), -sinf(angle1 / 80));
    axes[3] =
        Vector3(-sinf(angle1 / 90), -sinf(angle1 / 90), +sinf(angle1 / 99));
    axes[4] =
        Vector3(-sinf(angle1 / 70), -sinf(angle1 / 80), -sinf(angle1 / 90));
    axes[5] =
        Vector3(-sinf(angle1 / 99), +sinf(angle1 / 70), +sinf(angle1 / 80));

    for (uint8_t i = 0; i < ATOMS; i++) {
      Vector3 v = axes[i].rotate(angle2, atoms[i]);
      v *= Vector3(5, 5, 5);
      v += Vector3(8, 8, 8);
      Color c = Color((hue16 >> 8) + (i * (255 / 6)), RainbowGradientPalette);
      Display::radiate(v, c, 1.0f);
    }
    if (timer_duration.update()) {
      task = task_state_t::INACTIVE;
    }
  }
};
#endif
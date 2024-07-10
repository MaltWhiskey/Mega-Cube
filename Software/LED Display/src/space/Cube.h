#ifndef CUBE_H
#define CUBE_H

#include "Animation.h"

class Cube : public Animation {
private:
  float angle;
  float angle_speed;
  float distance;
  float radius_start;
  float radius_max;

  Vector3 a = Vector3(-1, -1, -1);
  Vector3 b = Vector3(+1, -1, -1);
  Vector3 c = Vector3(-1, +1, -1);
  Vector3 d = Vector3(+1, +1, -1);
  Vector3 e = Vector3(-1, -1, +1);
  Vector3 f = Vector3(+1, -1, +1);
  Vector3 g = Vector3(-1, +1, +1);
  Vector3 h = Vector3(+1, +1, +1);
  Vector3 polygon[12][2] = { {a, b}, {c, d}, {a, c}, {b, d}, {e, f}, {g, h},
                            {e, g}, {f, h}, {a, e}, {b, f}, {c, g}, {d, h} };

  static constexpr auto& settings = config.animation.cube;

public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    angle = 0;
  }

  void draw(float dt) {
    hue16_speed = settings.hue_speed * 255;
    angle_speed = settings.angle_speed;
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
    uint8_t pixnr = 0;

    Quaternion q = Quaternion(angle, Vector3(0, 0, 1));
    if (angle > 6 * 360) angle -= 6 * 360;
    if (angle > 4 * 360) {
      q = Quaternion(angle, Vector3(0, 1, 0));
    }
    else if (angle > 2 * 360) {
      q = Quaternion(angle, Vector3(1, 1, 1));
    }

    for (uint16_t i = 0; i < 12; i++) {
      Vector3 v1 = q.rotate(polygon[i][0] * radius);
      Vector3 v2 = q.rotate(polygon[i][1] * radius);
      Vector3 n = v1 - v2;
      // Get the amount of steps needed
      float steps = 1 + max(abs(n.z), max(abs(n.x), abs(n.y)));
      // Get the increment vector for each dimension
      Vector3 inc = n / steps;
      // The vector is pointing in the direction of Red -> Yellow
      for (uint8_t j = 0; j <= steps; j++)
        radiate(v1 - (inc * j),
          Color(pixnr += 6, RainbowGradientPalette).scale(brightness),
          distance);
    }
  }
};
#endif

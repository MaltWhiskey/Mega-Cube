#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "Animation.h"
#include "Power/Math8.h"

class Accelerometer : public Animation {
 private:
  float radius;

 public:
  static constexpr auto &settings = config.animation.accelerometer;

  void init() {
    state = state_t::RUNNING;
    timer_running = settings.runtime;
    radius = settings.radius;
  }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    if (timer_running.update()) {
      state = state_t::INACTIVE;
    }
    if (state == state_t::ENDING) {
      state = state_t::INACTIVE;
    }

    auto &hid = config.hid.accelerometer;
    Vector3 v = Vector3(hid.x, hid.z, hid.y);
    if (v.magnitude() > 0)
      v.normalize();
    else {
      line(Vector3(1, 1, 1));
      line(Vector3(-1, 1, -1));
      return;
    }
    Quaternion ortho = Quaternion(90, Vector3::Y) * Quaternion(180, Vector3::X);
    line(ortho.rotate(v));

    Quaternion front = Quaternion(-90, Vector3::X) *
                       Quaternion(90, Vector3::Y) * Quaternion(180, Vector3::X);
    Vector3 pointer = front.rotate(v);

    if (pointer.z < 0) {
      pointer *= 1 / pointer.z;
      if (pointer.x > 1)
        pointer.x = 1;
      else if (pointer.x < -1)
        pointer.x = -1;
      if (pointer.y > 1)
        pointer.y = 1;
      else if (pointer.y < -1)
        pointer.y = -1;
      voxel(pointer * radius, Color::BLUE);
    }
  }
};
#endif
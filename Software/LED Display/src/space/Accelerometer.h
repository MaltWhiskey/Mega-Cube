#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "Animation.h"
#include "Power/Math8.h"

class Accelerometer : public Animation {
 private:
  Timer timer_duration;

 public:
  void init(float duration) {
    state = state_t::STARTING;
    timer_duration = duration;
  }

  void draw(float dt) {
    setMotionBlur(config.animation.accelerometer.motionBlur);
    if (timer_duration.update()) {
      state = state_t::INACTIVE;
    }

    Vector3 v = Vector3(config.hid.accelerometer.x, config.hid.accelerometer.z,
                        config.hid.accelerometer.y)
                    .normalized();
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
      voxel(pointer * -7.5f, Color::BLUE);
    }
  }
};
#endif
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "Animation.h"
#include "power/Math8.h"

class Accelerometer : public Animation {
 private:
  float radius;

 public:
  static constexpr auto &settings = config.animation.accelerometer;

  void init() {
    state = state_t::RUNNING;
    timer_running = settings.runtime;
  }

  void draw(float dt) {
    radius = settings.radius;
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();
    if (timer_running.update()) {
      state = state_t::INACTIVE;
    }
    if (state == state_t::ENDING) {
      state = state_t::INACTIVE;
    }

    auto &hid = config.devices.accelerometer;
    Vector3 v = Vector3(hid.x, hid.z, hid.y);
    if (v.magnitude() > 0)
      v.normalize();
    else {  // Demo  mode
      static float phase = 0;
      phase += dt * 4.0f;
      v = Vector3(sinf(phase), sinf(phase * 0.9f), sinf(phase * 1.1f))
              .normalize();
    }
    Quaternion ortho = Quaternion(90, Vector3::Y) * Quaternion(180, Vector3::X);
    line(ortho.rotate(v));

    Quaternion front = Quaternion(-90, Vector3::X) *
                       Quaternion(90, Vector3::Y) * Quaternion(180, Vector3::X);
    Vector3 pointer = front.rotate(v);

    if (pointer.z < 0) {
      pointer *= -1 / pointer.z;
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
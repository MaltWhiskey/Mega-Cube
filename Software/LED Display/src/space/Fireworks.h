#ifndef FIREWORKS_H
#define FIREWORKS_H
#include <stdint.h>

#include "Animation.h"

class Object2 {
 public:
  Vector3 position = Vector3(0, 0, 0);
  Vector3 velocity = Vector3(0, 0, 0);

  uint8_t hue;
  // (0.f - 1.0f) -> 0x00 - 0xff
  float brightness;
  // seconds to fade brightness to zero
  float seconds;

 public:
  Object2(Vector3 p = Vector3(0, 0, 0), Vector3 v = Vector3(0, 0, 0),
          uint8_t h = 0, float b = 1.0f, float s = 1.0f)
      : position(p), velocity(v), hue(h), brightness(b), seconds(s) {}
  Object2& move(const float dt, const Vector3 gravity);
};

class Fireworks : public Animation {
 private:
  uint8_t numDebris;
  Vector3 source;
  Vector3 target;
  Vector3 velocity;
  Vector3 gravity;
  Object2 missile;
  Object2 debris[100];
  uint8_t loops = 0;

 private:
  void init();
  bool draw(float dt);
};
#endif
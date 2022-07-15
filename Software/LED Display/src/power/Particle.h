#ifndef PARTICLE_H
#define PARTICLE_H

#include "Color.h"
#include "Math3D.h"

class Particle {
 public:
  Vector3 position = Vector3(0, 0, 0);
  Vector3 velocity = Vector3(0, 0, 0);

  // postition in color pallete
  uint8_t hue;
  // (0.0f - 1.0f) -> 0x00 - 0xff
  float brightness;
  // seconds to fade brightness to zero
  float seconds;

  Particle(Vector3 p = Vector3(0, 0, 0), Vector3 v = Vector3(0, 0, 0),
           uint8_t h = 0, float b = 1.0f, float s = 1.0f)
      : position(p), velocity(v), hue(h), brightness(b), seconds(s) {}

  Particle& move(const float dt);
  Particle& move(const float dt, const Vector3 gravity);
};
#endif
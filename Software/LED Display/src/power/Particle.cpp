#include "Particle.h"

Particle& Particle::move(const float dt) {
  position = position + velocity * dt;
  return *this;
}

Particle& Particle::move(const float dt, const Vector3 gravity) {
  position = position + velocity * dt;
  velocity = velocity + gravity * dt;
  return *this;
}
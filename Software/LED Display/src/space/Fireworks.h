#ifndef FIREWORKS_H
#define FIREWORKS_H

#include "Animation.h"

class Fireworks : public Animation {
 private:
  float radius;
  uint16_t numDebris;
  Vector3 source;
  Vector3 target;
  Vector3 velocity;
  Vector3 gravity;
  Particle missile;
  Particle debris[200];
  boolean exploded;

  static constexpr auto &settings = config.animation.fireworks;

 public:
  void init() {
    state = state_t::RUNNING;
    timer_running = settings.runtime;
    radius = settings.radius;
    fireArrow();
  }

  void fireArrow() {
    // calculate source normally divided
    source = Vector3(noise.nextGaussian(0.0f, 0.25f), -1.0f,
                     noise.nextGaussian(0.0f, 0.25f));
    // calculate target normally divided
    target = Vector3(noise.nextGaussian(0.0f, 0.25f),
                     noise.nextGaussian(0.8f, 0.10f),
                     noise.nextGaussian(0.0f, 0.25f));
    // Assign a time in seconds to reach the target
    float t = noise.nextGaussian(0.60f, 0.20f);
    // Determine directional velocities in pixels per second
    velocity = (target - source) / t;
    // Set missile source and velocity
    missile.position = source;
    missile.velocity = velocity;
    // Set some system gravity
    gravity = Vector3(0, -1.0f, 0);
    exploded = false;
  }

    void draw(float dt) {
    radius = settings.radius;
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();

    // Missile drawing mode
    if (!exploded) {
      Vector3 temp = missile.position;
      missile.move(dt, gravity);
      // If missile falls back to earth or moved past the target explode it
      if ((temp.y > missile.position.y) | (missile.position.y > target.y)) {
        // Activate explode drawing mode
        exploded = true;
        // If target is reached the missile is exploded and debris is formed
        numDebris = random((sizeof(debris) / sizeof(Particle)) / 2,
                           sizeof(debris) / sizeof(Particle));
        // Overall exploding power of particles for all debris
        float pwr = noise.nextRandom(0.50f, 1.00f);
        // starting position in the hue color palette
        uint8_t hue = (uint8_t)random(0, 256);
        // generate debris with power and hue
        for (uint16_t i = 0; i < numDebris; i++) {
          // Debris has random velocities depending on overall power
          Vector3 explode =
              Vector3(noise.nextRandom(-pwr, pwr), noise.nextRandom(-pwr, pwr),
                      noise.nextRandom(-pwr, pwr));
          debris[i] = {temp, explode, uint8_t(hue + random(0, 64)), 1.0f,
                       noise.nextRandom(1.0f, 2.0f)};
        }
      }
      else {
        voxel(missile.position * radius, Color::WHITE);
      }
    }

    // Explosion drawing mode
    if (exploded) {
      uint16_t visible = 0;
      for (uint16_t i = 0; i < numDebris; i++) {
        if (debris[i].position.y > -1.0f)
          debris[i].move(dt, gravity);
        else
          debris[i].position.y = -1.0f;
        if (debris[i].brightness > 0) {
          visible++;
          debris[i].brightness -= dt * (1 / debris[i].seconds);
        } else {
          debris[i].brightness = 0;
        }
        Color c = Color(debris[i].hue, RainbowGradientPalette);
        // Add some random sparkles
        if (random(0, 20) == 0) {
          c = Color::WHITE;
        }
        c.scale(debris[i].brightness * brightness);
        voxel_add(debris[i].position * radius, c);
      }
      if (timer_running.update()) {
        state = state_t::ENDING;
      }
      if (visible == 0) {
        if (state == state_t::ENDING)
          state = state_t::INACTIVE;
        else
          fireArrow();
      }
    }
  }
};
#endif
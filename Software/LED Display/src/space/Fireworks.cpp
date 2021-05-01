#include "Fireworks.h"

void Fireworks::init() {
  if (loops == 0) {
    loops = random(3, 8);
  }
  // calculate source normally divided from source area
  source = Vector3(noise.nextGaussian((float)display.width / 2.0f, 2.0f), 0,
                   noise.nextGaussian((float)display.depth / 2.0f, 2.0f));
  // calculate targets normally divided from target area
  target =
      Vector3(noise.nextGaussian((float)display.width / 2.0f, 2.0f),
              noise.nextGaussian((float)8.0f * display.height / 10.0f, 1.0f),
              noise.nextGaussian((float)display.depth / 2.0f, 2.0f));
  // Assign a time in seconds to reach the target
  float t = noise.nextGaussian(0.60f, 0.25f);
  // Determine directional velocities in pixels per second
  velocity = (target - source) / t;
  // Set missile source and velocity
  missile.position = source;
  missile.velocity = velocity;
  // Set some system gravity
  gravity = Vector3(0, -8.0f, 0);
}

bool Fireworks::draw(float dt) {
  display.clear();
  // Missile drawing mode
  if (target.y > 0) {
    Vector3 temp = missile.position;
    missile.move(dt, gravity);
    // If missile falls back to earth or moved past the target explode the
    // missile
    if ((temp.y > missile.position.y) | (missile.position.y > target.y)) {
      // Activate explode drawing mode
      target.y = 0;
      // If target is reached the missile is exploded and debris is formed
      numDebris = random((sizeof(debris) / sizeof(Object2)) / 2,
                         sizeof(debris) / sizeof(Object2));
      // Overall exploding power of particles for all debris
      float pwr = noise.nextRandom(4.0f, 8.0f);
      // starting position in the hue colorwheel
      uint8_t hue = (uint8_t)random(0, 256);
      // generate debris with power and hue
      for (int i = 0; i < numDebris; i++) {
        // Debris is formed at target and has random velocities depending on
        // overall power
        Vector3 explode =
            Vector3(noise.nextRandom(-pwr, pwr), noise.nextRandom(-pwr, pwr),
                    noise.nextRandom(-pwr, pwr));
        debris[i] = {temp, explode, uint8_t(hue + random(0, 64)), 1.0f,
                     noise.nextRandom(1.0f, 2.0f)};
      }
    } else if (missile.position.inside(
                   Vector3(0, 0, 0),
                   Vector3(display.width, display.height, display.depth)))
      display.color(missile.position.x, missile.position.y,
                    missile.position.z) = Color(Color::WHITE);
  }
  // Explosion drawing mode
  if (target.y == 0) {
    uint8_t visible = 0;
    for (uint8_t i = 0; i < numDebris; i++) {
      if (debris[i].position.y > 0)
        debris[i].move(dt, gravity);
      else
        debris[i].position.y = 0;
      if (debris[i].brightness > 0) {
        visible++;
        debris[i].brightness -= dt * (1 / debris[i].seconds);
      } else {
        debris[i].brightness = 0;
      }
      Color c = Color(debris[i].hue, RainbowGradientPalette);
      c.r *= debris[i].brightness;
      c.g *= debris[i].brightness;
      c.b *= debris[i].brightness;
      if (debris[i].position.inside(
              Vector3(0, 0, 0),
              Vector3(display.width, display.height, display.depth)))
        display.color(debris[i].position.x, debris[i].position.y,
                      debris[i].position.z) += c;
    }
    if (visible == 0) {
      if (--loops == 0)
        return true;
      else
        init();
    }
  }
  return false;
}

/*------------------------------------------------------------------------------
 * OBJECT CLASS
 *------------------------------------------------------------------------------
 * The object class represents an object with a position, velocity and gravity
 * Forces get applied over delta time (float dt) in seconds.
 *----------------------------------------------------------------------------*/
Object2& Object2::move(const float dt, const Vector3 gravity) {
  position = position + velocity * dt;
  velocity = velocity + gravity * dt;
  return *this;
}
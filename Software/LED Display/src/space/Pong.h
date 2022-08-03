#ifndef PONG_H
#define PONG_H
#include "Animation.h"
class Pad {
 private:
  Vector3 position;
  Vector3 size;
  Color color;
  Vector3 velocity;
  Particle debris[100];
  bool exploded;

 public:
  Pad(Vector3 p = Vector3(0, 0, 0), Vector3 s = Vector3(0, 0, 0),
      Color c = Color::RED)
      : position(p), size(s), color(c) {}

  bool move(const float dt, uint8_t scale) {
    if (!exploded) {  // Exploded pads don't move
      auto& hid = config.hid.accelerometer;
      Vector3 v = Vector3(hid.x, hid.z, hid.y);
      if (v.magnitude() > 0)
        v.normalize();
      else {
        v = Vector3(0, 1, 0);
      }
      Quaternion front = Quaternion(-90, Vector3::X) *
                         Quaternion(90, Vector3::Y) *
                         Quaternion(180, Vector3::X);
      Vector3 pointer = front.rotate(v);
      if (pointer.z < 0) {
        pointer *= -1 / pointer.z;
        if (pointer.x > 1 - size.x)
          pointer.x = 1 - size.x;
        else if (pointer.x < -1)
          pointer.x = -1;
        if (pointer.y > 1 - size.y)
          pointer.y = 1 - size.y;
        else if (pointer.y < -1)
          pointer.y = -1;
        position = pointer;
      }
      // Draw the pad with new position
      Vector3 min = position * 7.5f;
      Vector3 max = (position + size) * 7.5f;
      for (float x = min.x; x <= max.x; x++)
        for (float y = min.y; y <= max.y; y++)
          for (float z = min.z; z <= max.z; z++) {
            voxel(Vector3(x, y, z), color.scaled(scale));
          }
    } else {  // Draw the debis of the exploded pad
      uint16_t visible = 0;
      for (uint16_t i = 0; i < sizeof(debris) / sizeof(Particle); i++) {
        debris[i].move(dt);
        if (debris[i].brightness > 0) {
          visible++;
          debris[i].brightness -= dt * (1 / debris[i].seconds);
        } else {
          debris[i].brightness = 0;
        }
        Color c = Color(debris[i].hue, RainbowGradientPalette);
        c.scale(debris[i].brightness * 255);
        voxel_add(debris[i].position * 7.5f, c);
      }
      // When explosion is finished reset the pad and ball
      return (visible == 0);
    }
    return false;
  }

  // Set pad at initial position
  void init() {
    size = Vector3(0.55f, 0.55f, 0.10f);
    position = Vector3(-size.x / 2, -size.y / 2, -1.0);
    exploded = false;
  }

  // If the ball is missed, explode the pad once else deflect
  void deflect(Vector3 ball, Vector3& velocity) {
    if (!exploded && ball.z < position.z) {
      exploded = !((position.x < ball.x) && ((position.x + size.x) > ball.x) &&
                   (position.y < ball.y) && ((position.y + size.y) > ball.y));
      if (exploded) generate_explosion();
      // deflect the ball using new velocities
      else {
        Vector3 center_pad = position + (size / 2);
        float dx = (center_pad.x - ball.x) / (size.x / 2);
        float dy = (center_pad.y - ball.y) / (size.y / 2);
        velocity.x = sinf(2 * PI * 45 / 360 * dx);
        velocity.y = cosf(2 * PI * 45 / 360 * dy);
        velocity.z = -velocity.z * 1.5f;
        velocity.normalize() * 2.3f;
      };
    }
  }

  void generate_explosion() {
    uint8_t hue = 0;
    float pwr = 3.00f;
    for (uint16_t i = 0; i < sizeof(debris) / sizeof(Particle); i++) {
      Noise noise = Animation::noise;
      Vector3 explode =
          Vector3(noise.nextRandom(-pwr, pwr), noise.nextRandom(-pwr, pwr),
                  noise.nextRandom(0, pwr));
      float x = noise.nextRandom(position.x, position.x + size.x);
      float y = noise.nextRandom(position.y, position.y + size.y);
      float z = noise.nextRandom(position.z, position.z + size.z);
      debris[i] = {Vector3(x, y, z), explode, uint8_t(hue + random(0, 24)),
                   1.0f, noise.nextRandom(0.5f, 1.0f)};
    }
  }
};

class Ball {
 private:
  Vector3 position;
  Vector3 velocity;
  Color color;

 public:
  Ball(Vector3 p = Vector3(0, 0, 0), Vector3 v = Vector3(0, 0, 0),
       Color c = Color::BLUE)
      : position(p), velocity(v), color(c) {}

  void move(const float dt, Pad& pad, uint8_t scale, uint8_t hue) {
    Vector3 t = position + velocity * dt;
    if (t.x > 1 || t.x < -1) velocity.x = -velocity.x;
    if (t.y > 1 || t.y < -1) velocity.y = -velocity.y;
    if (t.z > 1) velocity.z = -velocity.z;
    pad.deflect(t, velocity);

    color = Color(hue, RainbowGradientPalette);
    position = position + velocity * dt;
    radiate5(position * 7.5f, color.scaled(scale), 2.0f);
  }

  void init() {
    position = Vector3(0, 0, -0.7f);
    // Make sure vector length equals 1 for normalized speed
    velocity = Vector3(0.0f, 0.0f, 1.0f).normalize() * 2.0f;
  }
};

class Pong : public Animation {
 private:
  Pad pad;
  Ball ball;

  static constexpr auto& settings = config.animation.pong;

 public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    hue16_speed = settings.hue_speed * 255;
    pad.init();
    ball.init();
  }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness;
    hue16 += dt * hue16_speed;

    if (state == state_t::STARTING) {
      if (timer_starting.update()) {
        state = state_t::RUNNING;
        timer_running.reset();
      } else {
        brightness *= timer_starting.ratio();
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
      } else {
        brightness *= (1 - timer_ending.ratio());
      }
    }

    if (pad.move(dt, brightness)) {
      pad.init();
      ball.init();
    } else {
      ball.move(dt, pad, brightness, hue16 >> 8);
    }
  }
};
#endif
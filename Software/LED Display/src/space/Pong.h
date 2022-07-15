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
      Vector3 v =
          Vector3(config.hid.accelerometer.x, config.hid.accelerometer.z,
                  config.hid.accelerometer.y)
              .normalized();

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
        velocity.normalize() * 2.0f;
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

  Timer timer_duration;
  float time_fading;
  float time_phase;
  int16_t hue_speed;

 public:
  void init(float duration, float fading, uint16_t hue_speed_) {
    state = state_t::RUNNING;
    timer_duration = duration;
    time_phase = 0.0f;
    time_fading = fading;
    hue_speed = hue_speed_;
    pad.init();
    ball.init();
  }

  void end() {
    if (state == state_t::RUNNING) {
      state = state_t::ENDING;
      time_phase = time_fading;
    }
  }

  void draw(float dt) {
    setMotionBlur(config.animation.pong.motionBlur);
    hue16 += (int16_t)(255 * hue_speed * dt);
    if (timer_duration.update()) {
      state = state_t::ENDING;
      time_phase = time_fading;
    }
    if (state == state_t::ENDING) {
      time_phase -= dt;
      if (time_phase <= 0) {
        time_phase = 0;
        state = state_t::INACTIVE;
      }
    } else {
      time_phase += dt;
    }
    float scale = 255;
    if (time_phase <= time_fading) {
      scale = 255 * time_phase / time_fading;
    }
    if (pad.move(dt, scale)) {
      pad.init();
      ball.init();
      time_phase = 0.0f;
    } else {
      ball.move(dt, pad, scale, hue16 >> 8);
    }
  }
};
#endif
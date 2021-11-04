#ifndef TWINKELS_H
#define TWINKELS_H

#include "Animation.h"

class Twinkels : public Animation {
 private:
  // amount of time this animation keeps spawning
  Timer timer = 20.0f;
  // amount of seconds before spawning a new pixel
  Timer spawner = 0.1f;
  // amount of seconds it takes to fade a pixel to max
  float fade_in_speed = 1.0f;
  // amount of seconds it takes to fade a pixel to min
  float fade_out_speed = 2.0f;
  // store the source color
  Color colors[display.width][display.height][display.depth] = {};
  // amount of time the pixel is activated
  float delta[display.width][display.height][display.depth] = {};

 public:
  void init(float time, anim_t start, anim_t run, anim_t end) {
    task = RUNNING;
    timer = time;
    start_mode = start;
    run_mode = run;
    end_mode = end;
  }
  void speed(float spawn, float fade_in_speed_, float fade_out_speed_) {
    spawner = spawn;
    fade_in_speed = fade_in_speed_;
    fade_out_speed = fade_out_speed_;
  }
  void clear() {
    for (uint8_t x = 0; x < display.width; x++)
      for (uint8_t y = 0; y < display.height; y++)
        for (uint8_t z = 0; z < display.depth; z++) {
          delta[x][y][z] = 0;
          colors[x][y][z] = Color::BLACK;
        }
  }
  void draw(float dt) {
    bool pixels_active = false;
    for (uint8_t x = 0; x < display.width; x++) {
      for (uint8_t y = 0; y < display.height; y++) {
        for (uint8_t z = 0; z < display.depth; z++) {
          if (!colors[x][y][z].isBlack()) {
            if (delta[x][y][z] < fade_in_speed) {
              float t = delta[x][y][z] / fade_in_speed;
              Color c = colors[x][y][z];
              display.color(x, y, z) = c.scale(255 * t);
              delta[x][y][z] += dt;
            } else if (delta[x][y][z] < fade_in_speed + fade_out_speed) {
              float t = (delta[x][y][z] - fade_in_speed) / fade_out_speed;
              Color c = colors[x][y][z];
              display.color(x, y, z) = c.scale(255 * (1 - t));
              delta[x][y][z] += dt;
            } else {
              delta[x][y][z] = 0;
              colors[x][y][z] = Color::BLACK;
              display.color(x, y, z) = Color::BLACK;
            }
            pixels_active = true;
          }
        }
      }
    }
    // if this animation is finished start end mode
    if (timer.update()) {
      task = ENDING;
    }
    if (task != ENDING) {
      // Consider adding a new random twinkle
      if (spawner.update()) {
        uint8_t x = random(0, display.width);
        uint8_t y = random(0, display.height);
        uint8_t z = random(0, display.depth);
        if (delta[x][y][z] == 0) {
          switch (run_mode) {
            case COLOR:
              colors[x][y][z] = Color(0, 255);
              break;
            case WHITE:
              colors[x][y][z] = Color(255, 150, 30);
              break;
            default:
              break;
          }
        }
      }
    } else if (end_mode == FADE) {
      // deactivate when all pixels are black
      if (!pixels_active) task = INACTIVE;
      // deactivate immediately
    } else if (end_mode == NONE)
      task = INACTIVE;
  }
};
#endif
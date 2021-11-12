#ifndef TWINKELS_H
#define TWINKELS_H

#include "Animation.h"

class Twinkels : public Animation {
 private:
  // amount of time this animation keeps running
  Timer timer_duration = 20.0f;
  // amount of time before adding a new pixel
  Timer timer_interval = 0.01f;
  // amount of seconds it takes to fade a pixel to max
  float fade_in_speed = 1.0f;
  // amount of seconds it takes to fade a pixel to min
  float fade_out_speed = 3.0f;
  // source buffer for original value
  Color colors[Display::width][Display::height][Display::depth] = {};
  // amount of time the pixel is activated
  float time[Display::width][Display::height][Display::depth] = {};
  // single color for single color mode
  Color single_color;

 private:
  // different animation modes
  boolean mode_single_color = true;
  boolean mode_fade_out = true;

 public:
  void init(float duration, boolean single = false, boolean fade_out = false) {
    task = task_state_t::RUNNING;
    timer_duration = duration;
    mode_single_color = single;
    mode_fade_out = fade_out;
  }
  void color(Color c) { single_color = c; }
  void speed(float interval, float in_speed, float out_speed) {
    timer_interval = interval;
    fade_in_speed = in_speed;
    fade_out_speed = out_speed;
  }
  void clear() {
    for (uint8_t x = 0; x < Display::width; x++)
      for (uint8_t y = 0; y < Display::height; y++)
        for (uint8_t z = 0; z < Display::depth; z++) {
          time[x][y][z] = 0;
          colors[x][y][z] = Color::BLACK;
        }
  }
  void draw(float dt) {
    bool pixels_active = false;
    for (uint8_t x = 0; x < Display::width; x++) {
      for (uint8_t y = 0; y < Display::height; y++) {
        for (uint8_t z = 0; z < Display::depth; z++) {
          if (!colors[x][y][z].isBlack()) {
            if (time[x][y][z] < fade_in_speed) {
              float t = time[x][y][z] / fade_in_speed;
              Color c = colors[x][y][z];
              Display::cube[x][y][z] = c.scale(255 * t);
              time[x][y][z] += dt;
            } else if (time[x][y][z] < fade_in_speed + fade_out_speed) {
              float t = (time[x][y][z] - fade_in_speed) / fade_out_speed;
              Color c = colors[x][y][z];
              Display::cube[x][y][z] = c.scale(255 * (1 - t));
              time[x][y][z] += dt;
            } else {
              time[x][y][z] = 0;
              colors[x][y][z] = Color::BLACK;
              Display::cube[x][y][z] = Color::BLACK;
            }
            pixels_active = true;
          }
        }
      }
    }
    if (timer_duration.update()) {
      task = task_state_t::ENDING;
    }
    // Consider adding a new random twinkle
    if (task != task_state_t::ENDING) {
      if (timer_interval.update()) {
        uint8_t x = random(0, Display::width);
        uint8_t y = random(0, Display::height);
        uint8_t z = random(0, Display::depth);
        if (time[x][y][z] == 0) {
          if (mode_single_color) {
            colors[x][y][z] = single_color;
          } else {
            colors[x][y][z] = Color(0, 255);
          }
        }
      }
    } else if (mode_fade_out) {
      if (!pixels_active) {
        task = task_state_t::INACTIVE;
      }
    } else {
      task = task_state_t::INACTIVE;
    }
  }
};
#endif
#ifndef TWINKELS_H
#define TWINKELS_H

#include "Animation.h"

// source buffer for original value
DMAMEM Color colors[Display::width][Display::height][Display::depth];
// time the pixel is activated
DMAMEM float duration[Display::width][Display::height][Display::depth];

class Twinkels : public Animation {
 private:
  // amount of time before adding a new pixel
  Timer timer_interval;
  // amount of seconds it takes to fade a pixel to max
  float fade_in_speed;
  // amount of seconds it takes to fade a pixel to min
  float fade_out_speed;
  // single color for single color mode
  Color single_color;
  // different animation modes
  boolean mode_single_color;
  boolean mode_fade_out;

  static constexpr auto &settings = config.animation.twinkels;

 public:
  Twinkels() { set_clear(); }
  void init() {
    state = state_t::RUNNING;
    timer_running = settings.runtime;
    timer_interval = settings.interval;
    fade_in_speed = settings.fade_in_speed;
    fade_out_speed = settings.fade_out_speed;
  }
  void set_mode(bool single, bool fade_out) {
    mode_single_color = single;
    mode_fade_out = fade_out;
  }
  void set_color(Color c) { single_color = c; }
  void set_clear() {
    for (uint8_t x = 0; x < Display::width; x++)
      for (uint8_t y = 0; y < Display::height; y++)
        for (uint8_t z = 0; z < Display::depth; z++) {
          duration[x][y][z] = 0;
          colors[x][y][z] = Color::BLACK;
        }
  }
  void end() {
    if (state == state_t::RUNNING) {
      state = state_t::ENDING;
      mode_fade_out = true;
    }
  }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness;
    uint16_t pixels_active = 0;

    for (uint8_t x = 0; x < Display::width; x++) {
      for (uint8_t y = 0; y < Display::height; y++) {
        for (uint8_t z = 0; z < Display::depth; z++) {
          if (!colors[x][y][z].isBlack()) {
            if (duration[x][y][z] < fade_in_speed) {
              float t = duration[x][y][z] / fade_in_speed;
              voxel(x, y, z, colors[x][y][z].scaled(brightness * t));
              duration[x][y][z] += dt;
              pixels_active++;
            } else if (duration[x][y][z] < (fade_in_speed + fade_out_speed)) {
              float t = (duration[x][y][z] - fade_in_speed) / fade_out_speed;
              voxel(x, y, z, colors[x][y][z].scaled(brightness * (1 - t)));
              duration[x][y][z] += dt;
              pixels_active++;
            } else {
              duration[x][y][z] = 0;
              colors[x][y][z] = Color::BLACK;
              voxel(x, y, z, Color::BLACK);
            }
          }
        }
      }
    }
    if (timer_running.update()) {
      state = state_t::ENDING;
    }
    // Consider adding a new random twinkle
    if (state != state_t::ENDING) {
      if (timer_interval.update()) {
        uint8_t x = random(0, Display::width);
        uint8_t y = random(0, Display::height);
        uint8_t z = random(0, Display::depth);
        if (duration[x][y][z] == 0) {
          if (mode_single_color) {
            colors[x][y][z] = single_color;
          } else {
            colors[x][y][z] = Color(0, 255);
          }
        }
      }
    } else if (mode_fade_out) {
      if (pixels_active == 0) {
        state = state_t::INACTIVE;
      }
    } else {
      state = state_t::INACTIVE;
    }
  }
};
#endif
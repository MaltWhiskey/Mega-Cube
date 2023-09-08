#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "Animation.h"

class Spectrum : public Animation {
 private:
  float amplitude[64] = {0};
  uint8_t rx, ry, rz;
  bool btn_A = false;
  bool btn_B = false;
  bool btn_C = false;

  static constexpr auto &settings = config.animation.spectrum;

 public:
  void rotate_voxel(Color c, uint8_t x, uint8_t y, uint8_t z) {
    uint8_t d = Display::width - 1;
    uint8_t t;
    for (uint8_t i = 0; i < rx; i++) {
      t = d - z;
      z = y;
      y = t;
    }
    for (uint8_t i = 0; i < ry; i++) {
      t = d - z;
      z = x;
      x = t;
    }
    for (uint8_t i = 0; i < rz; i++) {
      t = d - y;
      y = x;
      x = t;
    }
    voxel(x, y, z, c);
  }

  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    hue16_speed = settings.hue_speed * 255;
    setMotionBlur(settings.motionBlur);
  }

  void draw(float dt) {
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

    auto &hid = config.hid.button;
    if (btn_A != hid.a) {
      btn_A = hid.a;
      if (hid.a) rx = (rx + 1) & 3;
    }
    if (btn_B != hid.b) {
      btn_B = hid.b;
      if (hid.b) ry = (ry + 1) & 3;
    }
    if (btn_C != hid.c) {
      btn_C = hid.c;
      if (hid.c) rz = (rz + 1) & 3;
    }

    auto &fft = config.hid.fft;
    if (fft.updated) {
      fft.updated = false;
      for (uint8_t i = 0; i < 64; i++) {
        uint8_t a = fft.data[i] & 0x0F;
        if (a > amplitude[i]) {
          amplitude[i] = a;
        }
      }
    }

    // Adjust bars and draw Spectrum
    for (uint8_t i = 0; i < 64; i++) {
      uint8_t a = (int)round(amplitude[i]) & 0xF;
      amplitude[i] *= 0.99f;
      uint8_t z = (i >> 2) & 0xE;
      uint8_t x = (i << 1) & 0xF;
      for (uint8_t y = 0; y <= a; y++) {
        Color c = Color(y * 15 + x * 2 + 10, LavaPalette);
        Color d = Color((hue16 >> 8) + x * 5 + z * 5, RainbowGradientPalette);
        Color e = Color(25, c, d).scale(brightness);
        rotate_voxel(e, x, y, z);
        rotate_voxel(e, x, y, z + 1);
        rotate_voxel(e, x + 1, y, z);
        rotate_voxel(e, x + 1, y, z + 1);
      }
    }
  }
};
#endif
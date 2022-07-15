#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "Animation.h"

class Spectrum : public Animation {
 private:
  Timer timer_duration;
  float amplitude[64] = {0};
  int16_t hue_speed;
  uint8_t rx, ry, rz;
  float time_phase = 0;
  float time_fading = 2.0f;
  float fft_updated = 0;
  float button_updated = 0;

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
    voxel(x, y, z, c.scale((1 - (time_phase / time_fading)) * 255));
  }

  void init(float duration, uint16_t hue_speed_) {
    timer_duration = duration;
    hue_speed = hue_speed_;
    state = state_t::RUNNING;
    time_phase = 0;
    time_fading = 2.0f;
  }

  void draw(float dt) {
    setMotionBlur(config.animation.spectrum.motionBlur);
    hue16 += (int16_t)(255 * hue_speed * dt);
    if (config.hid.button.updated != button_updated) {
      button_updated = config.hid.button.updated;
      if (config.hid.button.a) rx = (rx + 1) & 3;
      if (config.hid.button.b) ry = (ry + 1) & 3;
      if (config.hid.button.c) rz = (rz + 1) & 3;
    }
    if (config.hid.fft.updated != fft_updated) {
      fft_updated = config.hid.fft.updated;
      for (uint8_t i = 0; i < 64; i++) {
        uint8_t a = config.hid.fft.data[i] & 0x0F;
        if (a > amplitude[i]) {
          amplitude[i] = a;
        }
      }
    }

    // Adjust bars and draw Spectrum
    for (uint8_t i = 0; i < 64; i++) {
      uint8_t a = round(amplitude[i]) & 0xF;
      amplitude[i] *= 0.99f;
      uint8_t z = (i >> 2) & 0xE;
      uint8_t x = (i << 1) & 0xF;
      for (uint8_t y = 0; y <= a; y++) {
        Color c = Color(y * 15 + x * 2 + 10, LavaPalette);
        Color d = Color((hue16 >> 8) + x * 5 + z * 5, RainbowGradientPalette);
        c = Color(25, c, d);
        rotate_voxel(c, x, y, z);
        rotate_voxel(c, x, y, z + 1);
        rotate_voxel(c, x + 1, y, z);
        rotate_voxel(c, x + 1, y, z + 1);
      }
    }
    if (timer_duration.update()) {
      state = state_t::ENDING;
    }
    if (state == state_t::ENDING) {
      time_phase += dt;
      if (time_phase > time_fading) {
        state = state_t::INACTIVE;
      }
    }
  }
};
#endif
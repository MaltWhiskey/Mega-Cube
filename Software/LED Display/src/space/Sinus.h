#ifndef SINUS_H
#define SINUS_H

#include "Animation.h"

class Sinus : public Animation {
 private:
  float x_min = -2;
  float x_max = 2;
  float z_min = -2;
  float z_max = 2;
  float phase = 0.0f;
  float phase_speed = 0.0f;
  int16_t hue_speed = 0;

  Timer timer;
  float time;
  float fade_in;
  float fade_out;

 public:
  void init(float fade_in_, float time_, float fade_out_) {
    task = STARTING;
    timer = fade_in_;

    phase = 0.0f;
    fade_in = fade_in_;
    time = time_;
    fade_out = fade_out_;
  }

  void speed(float phase_speed_, int16_t hue_speed_) {
    phase_speed = phase_speed_;
    hue_speed = hue_speed_;
  }

  void draw(float dt) {
    phase += phase_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);

    uint8_t scale = 0;
    if (task == ENDING) {
      if (timer.update()) {
        task = INACTIVE;
        scale = 0;
      } else
        scale = (1 - timer.percent()) * 255;
    }
    if (task == RUNNING) {
      if (timer.update()) {
        task = ENDING;
        timer = fade_out;
      }
      scale = 255;
    }
    if (task == STARTING) {
      if (timer.update()) {
        task = RUNNING;
        timer = time;
        scale = 255;
      } else
        scale = timer.percent() * 255;
    }

    for (uint8_t x = 0; x < display.width; x++) {
      // convert cube x to floating point coordinate between x_min and x_max
      float xprime = mapf(x, 0, display.width - 1, x_min, x_max);
      for (uint8_t z = 0; z < display.depth; z++) {
        // convert cube z to floating point coordinate between z_min and z_max
        float zprime = mapf(z, 0, display.depth - 1, z_min, z_max);
        // determine y floating point coordinate
        float yprime = sinf(phase + sqrtf(xprime * xprime + zprime * zprime));
        // convert floating point y back to cube y
        float y = mapf(yprime, -1, 1, 0, display.height - 1);
        // display voxel on the cube
        Color c = Color((hue16 >> 8) + y * 10, &RainbowGradientPalette[0]);
        display.radiate(Vector3(x, z, y), c.scale(scale), 1.0f);
      }
    }
  }
};
#endif
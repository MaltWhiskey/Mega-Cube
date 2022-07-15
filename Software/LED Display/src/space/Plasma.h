#ifndef PLASMA_H
#define PLASMA_H

#include "Animation.h"
#include "Power/Math8.h"
#include "Power/Noise.h"
/*---------------------------------------------------------------------------------------
 * Noise
 *-------------------------------------------------------------------------------------*/
class Plasma : public Animation {
 private:
  Timer timer;
  float time_duration;
  float time_fade_out;
  uint8_t brightness = 100;

  // start somewhere in the noise map
  float noise_x = noise.nextRandom(0, 255);
  float noise_y = noise.nextRandom(0, 255);
  float noise_z = noise.nextRandom(0, 255);
  float noise_w = noise.nextRandom(0, 255);
  // Allocate noise memory
  uint8_t noise_map[Display::width][Display::height][Display::depth];
  // speed_offset is used to travel a 1d noise map and get the axis speeds
  float speed_offset = 0;
  float speed_offset_speed = 0.02f;
  // scale_p represents the distance between each pixel in the noise map
  float scale_p = 0.13f;
  // speed is how fast the movement is over the axis in the noise map
  float speed_x = 1.2f;
  float speed_y = 1.3f;
  float speed_z = 1.4f;
  float speed_w = 2.2f;

 public:
  void init(float fade_in, float duration, float fade_out) {
    state = state_t::STARTING;
    timer = fade_in;
    time_duration = duration;
    time_fade_out = fade_out;
  }

  void end() {
    if (state == state_t::RUNNING) {
      state = state_t::ENDING;
      timer = 2.0f;
    }
  }

  void draw(float dt) {
    setMotionBlur(config.animation.plasma.motionBlur);
    uint8_t scale = 0;
    if (state == state_t::ENDING) {
      if (timer.update()) {
        state = state_t::INACTIVE;
        scale = 0;
      } else
        scale = (1 - timer.percent()) * 255;
    }
    if (state == state_t::RUNNING) {
      if (timer.update()) {
        state = state_t::ENDING;
        timer = time_fade_out;
      }
      scale = 255;
    }
    if (state == state_t::STARTING) {
      if (timer.update()) {
        state = state_t::RUNNING;
        timer = time_duration;
        scale = 255;
      } else
        scale = timer.percent() * 255;
    }
    updateNoise(dt);
    makeNoise();
    drawNoise(scale);
  }

  // Create the 3D noise data matrix[][][]
  void makeNoise() {
    for (int x = 0; x < Display::width; x++) {
      float xoffset = noise_x + scale_p * x;
      for (int y = 0; y < Display::height; y++) {
        float yoffset = noise_y + scale_p * y;
        for (int z = 0; z < Display::depth; z++) {
          float zoffset = noise_z + scale_p * z;
          noise_map[x][y][z] =
              noise.noise4(xoffset, yoffset, zoffset, noise_w) * 255;
        }
      }
    }
  }

  // Draw the 3D noise data matrix[][][]
  void drawNoise(uint8_t scale) {
    for (int x = 0; x < Display::width; x++) {
      for (int y = 0; y < Display::height; y++) {
        for (int z = 0; z < Display::depth; z++) {
          // The index at (x,y,z) is the index in the color palette
          uint8_t index = noise_map[x][y][z];
          // The value at (y,x,z) is the overlay for the brightness
          voxel(x, y, z,
                Color((hue16 >> 8) + index, LavaPalette)
                    .scale(noise_map[y][x][z])
                    .scale(scale)
                    .scale(brightness));
        }
      }
    }
  }

  void updateNoise(float dt) {
    speed_offset += dt * speed_offset_speed;
    // use same speed offset, but offset each in the noise map
    speed_x = 2 * (noise.noise1(speed_offset + 000) - 0.5);    //  -1 to 1
    speed_y = 2 * (noise.noise1(speed_offset + 050) - 0.5);    //  -1 to 1
    speed_z = 2 * (noise.noise1(speed_offset + 100) - 0.5);    //  -1 to 1
    speed_w = 2 * (noise.noise1(speed_offset + 150) - 0.5);    //  -1 to 1
    scale_p = .15 + (noise.noise1(speed_offset + 200) / 6.6);  // .15 to .30

    // hue16 += (int16_t)(255 * config.noise.hue_speed * dt);

    noise_x += speed_x * dt;
    noise_y += speed_y * dt;
    noise_z += speed_z * dt;
    noise_w += speed_w * dt;
  }
};
#endif
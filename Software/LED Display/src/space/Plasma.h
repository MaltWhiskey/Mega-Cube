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
  // speed_offset is used to travel a 1d noise map and get the axis speeds
  float speed_offset;
  float speed_offset_speed;
  // scale_p represents the distance between each pixel in the noise map
  float scale_p;
  // speed is how fast the movement is over the axis in the noise map
  float speed_x;
  float speed_y;
  float speed_z;
  float speed_w;

  // start somewhere in the noise map
  float noise_x = noise.nextRandom(0, 255);
  float noise_y = noise.nextRandom(0, 255);
  float noise_z = noise.nextRandom(0, 255);
  float noise_w = noise.nextRandom(0, 255);
  // Allocate noise memory
  uint8_t noise_map[Display::width][Display::height][Display::depth];

  static constexpr auto &settings = config.animation.plasma;

 public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    scale_p = settings.scale_p;
    speed_x = settings.speed_x;
    speed_y = settings.speed_y;
    speed_z = settings.speed_z;
    speed_w = settings.speed_w;
    speed_offset = 0;
    speed_offset_speed = settings.speed_offset_speed;
  }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness;

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
    updateNoise(dt);
    makeNoise();
    drawNoise(brightness);
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
  void drawNoise(uint8_t brightness) {
    for (int x = 0; x < Display::width; x++) {
      for (int y = 0; y < Display::height; y++) {
        for (int z = 0; z < Display::depth; z++) {
          // The index at (x,y,z) is the index in the color palette
          uint8_t index = noise_map[x][y][z];
          // The value at (y,x,z) is the overlay for the brightness
          voxel(x, y, z,
                Color((hue16 >> 8) + index, LavaPalette)
                    .scale(noise_map[y][x][z])
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

    noise_x += speed_x * dt;
    noise_y += speed_y * dt;
    noise_z += speed_z * dt;
    noise_w += speed_w * dt;
  }
};
#endif
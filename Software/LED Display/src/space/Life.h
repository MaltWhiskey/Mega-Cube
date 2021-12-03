#ifndef LIFE_H
#define LIFE_H

#include "Animation.h"
#include "Power/Math8.h"

class Life : public Animation {
 private:
  Timer timer_duration;
  Timer timer_interval = 10.0f;
  int16_t hue_speed = 1000;
  uint8_t previous_cells[16][16][16] = {};
  uint8_t current_cells[16][16][16] = {};
  Color colors[Display::width][Display::height][Display::depth] = {};
  uint16_t hue16 = 0;
  uint16_t alive[10] = {};
  // uint8_t R[4] = {4, 5, 5, 5};
  uint8_t R[4] = {5, 7, 6, 7};
  //  uint8_t R[4] = {2, 3, 3, 3};

 public:
  void init() {
    task = task_state_t::RUNNING;
    timer_duration = 460.0f;
    randomize();
  }

  void randomize() {
    for (uint8_t x = 0; x < Display::width; x++)
      for (uint8_t y = 0; y < Display::height; y++)
        for (uint8_t z = 0; z < Display::depth; z++) {
          if (random(0, 2) == 0) {
            current_cells[x][y][z] = 1;
            previous_cells[x][y][z] = 1;
          }
          colors[x][y][z] = Color::BLACK;
        }
    for (int8_t x = 0; x < Display::width; x++)
      for (int8_t y = 0; y < Display::height; y++)
        for (int8_t z = 0; z < Display::depth; z++) {
          uint8_t neighbours = 0;
          for (int8_t x_ = x - 1; x_ <= x + 1; x_++)
            for (int8_t y_ = y - 1; y_ <= y + 1; y_++)
              for (int8_t z_ = z - 1; z_ <= z + 1; z_++)
                neighbours += previous_cells[x_ & 0xf][y_ & 0xf][z_ & 0xf];

          if (previous_cells[x][y][z]) {
            if ((neighbours < R[0]) || (neighbours > R[1])) {
              current_cells[x][y][z] = 0;
            }
          } else {
            if ((neighbours >= R[2]) && (neighbours <= R[3])) {
              current_cells[x][y][z] = 1;
            }
          }
        }
    memcpy(previous_cells, current_cells, sizeof(previous_cells));

    for (int8_t x = 0; x < Display::width; x++)
      for (int8_t y = 0; y < Display::height; y++)
        for (int8_t z = 0; z < Display::depth; z++) {
          uint8_t neighbours = 0;
          for (int8_t x_ = x - 1; x_ <= x + 1; x_++)
            for (int8_t y_ = y - 1; y_ <= y + 1; y_++)
              for (int8_t z_ = z - 1; z_ <= z + 1; z_++)
                neighbours += previous_cells[x_ & 0xf][y_ & 0xf][z_ & 0xf];

          if (previous_cells[x][y][z]) {
            if ((neighbours < R[0]) || (neighbours > R[1])) {
              current_cells[x][y][z] = 0;
            }
          } else {
            if ((neighbours >= R[2]) && (neighbours <= R[3])) {
              current_cells[x][y][z] = 1;
            }
          }
        }
  }

  void draw(float dt) {
    uint16_t overflow = hue16 >> 8;
    hue16 += (int16_t)(255 * hue_speed * dt);
    uint16_t hue = hue16 >> 8;
    if (overflow > hue) {
      overflow = 256;
      hue = 255;
      for (uint16_t i = 1; i <= 9; i++) {
        alive[i - 1] = alive[i];
      }
    }

    uint16_t alive_cells = 0;
    for (uint8_t x = 0; x < Display::width; x++)
      for (uint8_t y = 0; y < Display::height; y++)
        for (uint8_t z = 0; z < Display::depth; z++) {
          Color b, c = colors[x][y][z];
          if (previous_cells[x][y][z] < current_cells[x][y][z])
            b = Color(hue, c, Color::BLUE);  // 01
          else if (previous_cells[x][y][z] > current_cells[x][y][z])
            b = Color(hue, c, Color::RED);  // 10
          else if (!previous_cells[x][y][z] & !current_cells[x][y][z])
            b = Color(hue, c, Color::BLACK);  // 00
          else
            b = c;
          Display::cube[x][y][z] = b;
          if (overflow > hue) colors[x][y][z] = b;
          if (!b.isBlack()) alive_cells++;
        }

    if (overflow > hue) {
      // make previous equal to current
      memcpy(previous_cells, current_cells, sizeof(previous_cells));

      for (int8_t x = 0; x < Display::width; x++)
        for (int8_t y = 0; y < Display::height; y++)
          for (int8_t z = 0; z < Display::depth; z++) {
            uint8_t neighbours = 0;
            for (int8_t x_ = x - 1; x_ <= x + 1; x_++)
              for (int8_t y_ = y - 1; y_ <= y + 1; y_++)
                for (int8_t z_ = z - 1; z_ <= z + 1; z_++)
                  neighbours += previous_cells[x_ & 0xf][y_ & 0xf][z_ & 0xf];

            if (previous_cells[x][y][z]) {
              if ((neighbours < R[0]) || (neighbours > R[1])) {
                current_cells[x][y][z] = 0;
              }
            } else {
              if ((neighbours >= R[2]) && (neighbours <= R[3])) {
                current_cells[x][y][z] = 1;
              }
            }
          }
    }

    if (!alive_cells) {
      randomize();
    }
    if (overflow > hue) {
      alive[9] = alive_cells;
      bool same = true;
      for (uint16_t i = 1; i <= 9; i++) {
        if (alive[i - 1] != alive[i]) same = false;
      }
      if (same) randomize();
    }
    if (timer_duration.update()) {
      task = task_state_t::INACTIVE;
    }
  }
};
#endif
#ifndef LIFE_H
#define LIFE_H

#include "Animation.h"
#include "Power/Math8.h"

class Life : public Animation {
 private:
  float time_phase;
  float time_interval;

  uint16_t cells_g1[16][16];
  uint16_t cells_g2[16][16];
  enum class rule_t : uint8_t { DIE = 0, LIVE = 1, BIRTH = 2 };
  rule_t rules[27];

  // Hash list containing the hashes of past generations
  uint32_t hash_list[256];
  // Amount of hashes available and position to store new  hash in
  uint32_t hash_nr;
  // The amount of living cells in the new generation
  uint16_t living = 0;
  // Sequence for generate different rules and start conditions
  uint8_t sequence = 0;

  static constexpr auto &settings = config.animation.life;

 public:
  template <typename T, size_t size_z, size_t size_y>
  // Project gliders starting from (0,15,0) for easy visualisation
  void generate_glider(T (&data)[size_z][size_y], uint16_t size_x, uint16_t x,
                       uint16_t y, uint16_t z) {
    for (uint16_t w = 0; w < size_x; w++)
      for (uint16_t h = 0; h < size_y; h++)
        for (uint16_t d = 0; d < size_z; d++) {
          if (data[d][h] & 1 << (size_x - 1 - w)) {
            cells_g2[(x + w) & 0xF][(15 - y - h) & 0xF] |= 1 << ((z + d) & 0xF);
            living++;
          }
        }
  }
  // Create some gliders for Life 4444
  void game_gliders() {
    uint16_t glider1[2][3] = {{0b0001,    // x = [0,1,2,3], y = [15], z = [0]
                               0b1000,    // x = [0,1,2,3], y = [14], z = [0]
                               0b0110},   // x = [0,1,2,3], y = [13], z = [0]
                              {0b0001,    // x = [0,1,2,3], y = [15], z = [1]
                               0b1000,    // x = [0,1,2,3], y = [14], z = [1]
                               0b0110}};  // x = [0,1,2,3], y = [13], z = [1]
    uint16_t glider2[2][4] = {{0b010,     // x = [0,1,2,3], y = [15], z = [0]
                               0b100,     // x = [0,1,2,3], y = [14], z = [0]
                               0b100,     // x = [0,1,2,3], y = [13], z = [0]
                               0b001},    // x = [0,1,2,3], y = [12], z = [0]
                              {0b010,     // x = [0,1,2,3], y = [15], z = [1]
                               0b100,     // x = [0,1,2,3], y = [14], z = [1]
                               0b100,     // x = [0,1,2,3], y = [13], z = [1]
                               0b001}};   // x = [0,1,2,3], y = [12], z = [1]
    generate_glider(glider1, 4, 0, 0, 0);
    generate_glider(glider1, 4, 6, 3, 0);
    generate_glider(glider1, 4, 12, 6, 0);
    generate_glider(glider2, 3, 0, 0, 8);
    generate_glider(glider2, 3, 6, 4, 8);
    generate_glider(glider2, 3, 12, 8, 8);
  }
  // Create primordial soup as the new generation
  void game_randomize(uint16_t amount, uint16_t radius) {
    for (int16_t x = 0; x < 16; x++)
      for (int16_t y = 0; y < 16; y++) {
        cells_g2[x][y] = 0;
      }
    // Leave the outer space empty and fill the rest with amout of living cells
    for (uint16_t i = 0; i < amount; i++) {
      // Get random polar coordinates somewhere within radius
      float r = noise.nextRandom(0, radius);
      float theta = noise.nextRandom(0, 2 * PI);
      float psi = noise.nextRandom(0, 2 * PI);
      // Convert from polar coordinates to Carthesian and offset to the center
      uint16_t x = roundf(7.5f + r * sinf(psi) * cosf(theta));
      uint16_t y = roundf(7.5f + r * sinf(psi) * sinf(theta));
      uint16_t z = roundf(7.5f + r * cosf(psi));
      cells_g2[x][y] |= (1 << z);
    }
    // Overestimate of amount because of duplicates
    living = amount;
  }
  // Clear all generations and reset the game rules
  void game_reset() {
    living = hash_nr = 0;
    for (uint8_t x = 0; x < 16; x++)
      for (uint8_t y = 0; y < 16; y++) {
        cells_g1[x][y] = 0;
        cells_g2[x][y] = 0;
      }
    game_rule_reset();
  }
  // Reset the game rules
  void game_rule_reset() {
    for (uint8_t i = 0; i < 27; i++) {
      rules[i] = rule_t::DIE;
    }
  }
  // Count the neighbours of a given cell
  uint16_t count_neighbours(int16_t x_, int16_t y_, int16_t z_) {
    uint16_t neighbours = 0;
    for (int16_t x = x_ - 1; x <= x_ + 1; x++)
      for (int16_t y = y_ - 1; y <= y_ + 1; y++) {
        uint16_t cells = cells_g1[x & 0xF][y & 0xF];
        for (int16_t z = z_ - 1; z <= z_ + 1; z++) {
          uint16_t cell = cells & (1 << (z & 0xF));
          if (!(x == x_ && y == y_ && z == z_) && cell) {
            neighbours++;
          }
        }
      }
    return neighbours;
  }
  // Create a new generation using the game rules, calculates
  // hash value to detect cycles and amount of living cells.
  // These conditions can be used to start a new game if it's
  // spinning out of control, is in a cycle or all is extinct
  uint32_t game_next_generation() {
    uint16_t living = 0;
    uint32_t hash = 0;
    memcpy(cells_g1, cells_g2, sizeof(cells_g1));
    for (int16_t x = 0; x < 16; x++) {
      for (int16_t y = 0; y < 16; y++) {
        uint16_t cells = cells_g1[x][y];
        for (int16_t z = 0; z < 16; z++) {
          uint16_t count = count_neighbours(x, y, z);
          hash += count * (x * 3 + y * 5 + z * 7);
          if (rules[count] == rule_t::DIE)
            cells &= ~(1 << z);
          else if (rules[count] == rule_t::BIRTH)
            cells |= (1 << z);
          if (cells & (1 << z)) living++;
        }
        cells_g2[x][y] = cells;
      }
    }
    this->living = living;
    return hash;
  }
  // Main game progress loop, selects rules and start conditions.
  // Creates a new generation is all cells are dead. Also checks
  // for other ending conditions and if those are met all will die.
  void game_progress() {
    uint32_t hash = game_next_generation();
    if (living == 0) {
      game_reset();
      switch (sequence++) {
        case 0:  // Life 4444 gliders
          rules[4] = rule_t::BIRTH;
          game_gliders();
          break;
        case 1:  // Life 4555
          rules[4] = rule_t::LIVE;
          rules[5] = rule_t::BIRTH;
          game_randomize(random(200, 400), noise.nextRandom(5.0f, 7.0f));
          break;
        case 2:  // Life 5766
          rules[5] = rule_t::LIVE;
          rules[7] = rule_t::LIVE;
          rules[6] = rule_t::BIRTH;
          game_randomize(random(200, 400), noise.nextRandom(5.0f, 7.0f));
          break;
        case 3:  // Life 5655
          rules[5] = rule_t::BIRTH;
          rules[6] = rule_t::LIVE;
          game_randomize(random(200, 400), noise.nextRandom(5.0f, 7.0f));
          break;
        case 4:  // Life 5855
          sequence = 0;
          rules[5] = rule_t::BIRTH;
          rules[6] = rule_t::LIVE;
          rules[7] = rule_t::LIVE;
          rules[8] = rule_t::LIVE;
          game_randomize(25, 3.0f);
          break;
      }
    } else {
      // Dectect how many of current generation existed before. The hash
      // isn't deterministic, but there is only a tiny chance of the same
      // hash with different generations.
      uint8_t matches = 0;
      for (uint8_t i = 0; i < hash_nr && i <= 255; i++)
        if (hash_list[i] == hash) matches++;
      // Show only 1 cycles of custom gliders in life 4444
      if (sequence - 1 == 0 && matches >= 1) {
        game_rule_reset();
        game_next_generation();
      }
      // Allow maximum 6 duplicate hashes and no more than 500 living cells
      else if (matches >= 6 || living >= 500) {
        game_rule_reset();
        game_next_generation();
      } else {
        hash_list[hash_nr++ & 0xFF] = hash;
      }
    }
  }

  void init() {
    state = state_t::RUNNING;
    timer_running = settings.runtime;
    time_interval = settings.interval;
    time_phase = time_interval * 2;
    game_reset();
  }

  void draw(float dt) {
    setMotionBlur(config.animation.life.motionBlur);
    uint8_t brightness = settings.brightness;

    if (timer_running.update()) {
      state = state_t::ENDING;
    }

    time_phase += dt;
    float scale = 0;
    uint8_t index = 0;
    if (time_phase <= time_interval) {
      scale = 255 * time_phase / time_interval;
    } else if (time_phase <= 2 * time_interval) {
      index = 1;
      scale = 255 * (time_phase - time_interval) / time_interval;
    } else {
      time_phase = 0.0f;
      if (state != state_t::ENDING)
        game_progress();
      else if (living == 0) {
        state = state_t::INACTIVE;
        index = 1;
        scale = 255;
      } else {
        game_rule_reset();
        game_next_generation();
      }
    }

    Color alive = Color(255, 150, 30);
    Color sparkle = Color(255, 30, 150);
    Color dead = Color::BLACK;
    Color dieing = Color::RED;
    Color birth = Color::BLUE;

    Color colors[6] = {
        Color(scale, alive, sparkle), Color(scale, sparkle, alive),
        Color(scale, alive, dieing),  Color(scale, dieing, dead),
        Color(scale, dead, birth),    Color(scale, birth, alive)};

    for (int16_t x = 0; x < 16; x++)
      for (int16_t y = 0; y < 16; y++) {
        uint16_t g1 = cells_g1[x][y];
        uint16_t g2 = cells_g2[x][y];
        for (int16_t z = 0; z < 16; z++) {
          uint16_t i = index;
          uint16_t mask = (1 << z);
          if (g1 & g2 & mask) {
            i += 0;
          } else if (g1 & ~g2 & mask)
            i += 2;
          else if (~g1 & g2 & mask)
            i += 4;
          else
            continue;
          voxel(x, y, z, colors[i].scaled(brightness));
        }
      }
  }
};
#endif
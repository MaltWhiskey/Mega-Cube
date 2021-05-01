#include "Sinus.h"

void Sinus::init() {
  phase = 0;
  x1 = noise.nextRandom(-3, 0);
  x2 = noise.nextRandom(0, 3);
  z1 = noise.nextRandom(-3, 0);
  z2 = noise.nextRandom(0, 3);
}

bool Sinus::draw(float dt) {
  display.clear();
  phase += 3.5f * dt;
  colorwheel -= (uint16_t)(255 * 100 * dt);
  for (uint8_t x = 0; x < display.width; x++) {
    float x3, y3, z3;
    x3 = mapf(x, 0, display.width - 1, x1, x2);
    for (uint8_t z = 0; z < display.depth; z++) {
      z3 = mapf(z, 0, display.depth - 1, z1, z2);
      y3 = sinf(phase + sqrtf(x3 * x3 + z3 * z3));
      Vector3 v = Vector3(x, mapf(y3, -1, 1, 0, display.height - 1), z);
      Color c = Color((colorwheel >> 8) + v.y * 10, &RainbowGradientPalette[0]);
      display.color(round(v.x), round(v.y), round(v.z)) = c;
    }
  }
  return (phase / (12 * PI) >= 1);
}
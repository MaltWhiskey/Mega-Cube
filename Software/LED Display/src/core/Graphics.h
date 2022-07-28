#ifndef GRAPHICS_H_
#define GRAPHICS_H_
#include <Arduino.h>
#include <math.h>
#include <stdint.h>

#include "core/Display.h"
#include "power/Color.h"
#include "power/Math3D.h"

// Set voxel using physical cube coordinates
inline void voxel(const uint8_t x, const uint8_t y, const uint8_t z,
                  const Color &c) {
  if (((x | y | z) & 0xF0) == 0) {
    Display::cube[Display::cubeBuffer][x][y][z] = c;
  }
}

// Set voxel using system coordinates (translates to physical cube coordinates)
// Has correction for Vector3(7.5f, 7.5f, 7.5f) build in independent of v1
inline void voxel(const Vector3 &v0, const Color &c,
                  const Vector3 &v1 = Vector3(0, 0, 0)) {
  Vector3 v = v1 + v0;
  // Translate and round to nearest integer
  int16_t x = (int16_t)(v.x + 32768) - 32760;
  int16_t y = (int16_t)(v.y + 32768) - 32760;
  int16_t z = (int16_t)(v.z + 32768) - 32760;
  if (((x | y | z) & 0xFFF0) == 0) {
    Display::cube[Display::cubeBuffer][x][y][z] = c;
  }
}

// Set voxel using system coordinates (translates to physical cube coordinates)
// Has correction for Vector3(7.5f, 7.5f, 7.5f) build in independent of v1
inline void voxel_add(const Vector3 &v0, const Color &c,
                      const Vector3 &v1 = Vector3(0, 0, 0)) {
  Vector3 v = v1 + v0;
  // Translate and round to nearest integer
  int16_t x = (int16_t)(v.x + 32768) - 32760;
  int16_t y = (int16_t)(v.y + 32768) - 32760;
  int16_t z = (int16_t)(v.z + 32768) - 32760;
  if (((x | y | z) & 0xFFF0) == 0) {
    Display::cube[Display::cubeBuffer][x][y][z] += c;
  }
}

// Set voxel using system coordinates (translates to physical cube coordinates)
inline void radiate4(const Vector3 &v0, const Color &c, const float r,
                     const Vector3 &v1 = Vector3(7.5f, 7.5f, 7.5f)) {
  Vector3 v = v1 + v0;
  // Round lower bounds down and round higher bounds up
  int16_t x1 = (int16_t)(v.x - r + 32768) - 32767;
  int16_t x2 = (int16_t)(v.x + r + 32768) - 32768;
  int16_t y1 = (int16_t)(v.y - r + 32768) - 32767;
  int16_t y2 = (int16_t)(v.y + r + 32768) - 32768;
  int16_t z1 = (int16_t)(v.z - r + 32768) - 32767;
  int16_t z2 = (int16_t)(v.z + r + 32768) - 32768;

  for (int16_t x = x1; x <= x2; x++)
    for (int16_t y = y1; y <= y2; y++)
      for (int16_t z = z1; z <= z2; z++) {
        if (((x | y | z) & 0xFFF0) == 0) {
          float radius = (Vector3(x, y, z) - v).magnitude();
          // Scale color proportional to 1 / (1 + r^4)
          if (radius < r) {
            Display::cube[Display::cubeBuffer][x][y][z].maximize(
                c.scaled(255 / (1 + (radius * radius * radius * radius))));
          }
        }
      }
}
// Set voxel using system coordinates (translates to physical cube coordinates)
inline void radiate5(const Vector3 &v0, const Color &c, const float r,
                     const Vector3 &v1 = Vector3(7.5f, 7.5f, 7.5f)) {
  Vector3 v = v1 + v0;
  // Round lower bounds down and round higher bounds up
  int16_t x1 = (int16_t)(v.x - r + 32768) - 32767;
  int16_t x2 = (int16_t)(v.x + r + 32768) - 32768;
  int16_t y1 = (int16_t)(v.y - r + 32768) - 32767;
  int16_t y2 = (int16_t)(v.y + r + 32768) - 32768;
  int16_t z1 = (int16_t)(v.z - r + 32768) - 32767;
  int16_t z2 = (int16_t)(v.z + r + 32768) - 32768;

  for (int16_t x = x1; x <= x2; x++)
    for (int16_t y = y1; y <= y2; y++)
      for (int16_t z = z1; z <= z2; z++) {
        if (((x | y | z) & 0xFFF0) == 0) {
          float radius = (Vector3(x, y, z) - v).magnitude();
          // Scale color proportional to 1 / (1 + r^5)
          if (radius < r) {
            Display::cube[Display::cubeBuffer][x][y][z].maximize(c.scaled(
                255 / (1 + (radius * radius * radius * radius * radius))));
          }
        }
      }
}

// Set voxel using system coordinates (translates to physical cube coordinates)
inline void radiate(const Vector3 &v0, const Color &c, const float r,
                    const Vector3 &v1 = Vector3(7.5f, 7.5f, 7.5f)) {
  Vector3 v = v1 + v0;
  // Round lower bounds down and round higher bounds up
  int16_t x1 = (int16_t)(v.x - r + 32768) - 32767;
  int16_t x2 = (int16_t)(v.x + r + 32768) - 32768;
  int16_t y1 = (int16_t)(v.y - r + 32768) - 32767;
  int16_t y2 = (int16_t)(v.y + r + 32768) - 32768;
  int16_t z1 = (int16_t)(v.z - r + 32768) - 32767;
  int16_t z2 = (int16_t)(v.z + r + 32768) - 32768;

  for (int16_t x = x1; x <= x2; x++)
    for (int16_t y = y1; y <= y2; y++)
      for (int16_t z = z1; z <= z2; z++) {
        if (((x | y | z) & 0xFFF0) == 0) {
          float radius = (Vector3(x, y, z) - v).magnitude();
          // Scale color proportional to distance
          if (radius < r) {
            Display::cube[Display::cubeBuffer][x][y][z].maximize(
                c.scaled(255 * (1 - (radius / r))));
          }
        }
      }
}
// Draw a line through the center of the cube with direction n
inline void line(Vector3 n) {
  // Get the normal vector n hat.
  n.normalize();
  // Multiply by the diagonal corner to corner distance
  n *= sqrt(3) * 7.5f;
  // Get the amount of steps needed
  float steps = max(abs(n.z), max(abs(n.x), abs(n.y)));
  // Get the increment vector for each dimension
  Vector3 inc = n / steps;
  // The vector is pointing in the direction of Red -> Yellow
  for (uint8_t i = 0; i <= steps; i++) {
    voxel(inc * +i, Color(+i * 8, RainbowGradientPalette));
    voxel(inc * -i, Color(-i * 8, RainbowGradientPalette));
  }
}

inline void setMotionBlur(uint8_t n) { Display::setMotionBlur(n); }
#endif
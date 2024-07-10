#ifndef GRAPHICS_H_
#define GRAPHICS_H_
#include <Arduino.h>
#include <math.h>
#include <stdint.h>

#include "core/Display.h"
#include "power/Color.h"
#include "power/Math3D.h"
/*------------------------------------------------------------------------------
 * Grapics is used to draw voxels on the screen in a safe way
 *------------------------------------------------------------------------------
 * Rounding up/down doesn't work correctly with negative numbers.
 *  int16_t x1 = (int16_t)(v.x - r + 1);  // round up
 *  int16_t x2 = (int16_t)(v.x + r);      // round down
 * For example -2.3 -> -1 rounding up (-2) and -2 rounding down (-3)
 * For example -1.3 ->  0 rounding up (-1) and -1 rounding down (-2)
 * For example -0.3 ->  0 rounding up ( 0) and  0 rounding down (-1)
 * Since we ignore negative numbers, this will always be okay.
 *
 * If negative numbers need to be rounded correctly first make them positive
 * by adding a number, truncate and then substract that number-1 to round up
 *   int16_t x1 = (int16_t)(v.x - r + 32768.0f) - 32767;
 * by adding a number, truncate and then substract that number to round down
 *   int16_t x1 = (int16_t)(v.x - r + 32768.0f) - 32768;
 * Rounding to the nearest integer can be done like this
 *   int16_t x1 = (int16_t)(v.x + 32768.5f) - 32768;
 * *
 * Alternatively use this define with a comparison to round down
 *   #define FASTFLOOR(x) (((int)(x) < (x)) ? ((int)x) : ((int)x - 1))
 *----------------------------------------------------------------------------*/

// Define the center of the physical cube coordinates
#define CX 7.5f
#define CY 7.5f
#define CZ 7.5f

// Set voxel using physical cube coordinates
inline void voxel(const uint8_t x, const uint8_t y, const uint8_t z,
                  const Color &c) {
  if (((x | y | z) & 0xF0) == 0) {
    Display::cube[Display::cubeBuffer][x][y][z] = c;
  }
}

// Set voxel using system coordinates
inline void voxel(const Vector3 &v, const Color &c) {
  // Round to nearest integer and translate back
  int16_t x = (int16_t)(v.x + 32768.5f + CX) - 32768;
  int16_t y = (int16_t)(v.y + 32768.5f + CY) - 32768;
  int16_t z = (int16_t)(v.z + 32768.5f + CZ) - 32768;
  if (((x | y | z) & 0xFFF0) == 0) {
    Display::cube[Display::cubeBuffer][x][y][z] = c;
  }
}

// Set voxel using system coordinates
inline void voxel_add(const Vector3 &v, const Color &c) {
  // Round to nearest integer and translate back
  int16_t x = (int16_t)(v.x + 32768.5f + CX) - 32768;
  int16_t y = (int16_t)(v.y + 32768.5f + CY) - 32768;
  int16_t z = (int16_t)(v.z + 32768.5f + CZ) - 32768;
  if (((x | y | z) & 0xFFF0) == 0) {
    Display::cube[Display::cubeBuffer][x][y][z] += c;
  }
}

// Set voxel using system coordinates
inline void radiate(const Vector3 &v0, const Color &c, const float r) {
  Vector3 v = v0 + Vector3(CX, CY, CZ);
  int16_t x1 = (int16_t)(v.x - r + 1);  // round up
  int16_t x2 = (int16_t)(v.x + r);      // round down
  int16_t y1 = (int16_t)(v.y - r + 1);  // round up
  int16_t y2 = (int16_t)(v.y + r);      // round down
  int16_t z1 = (int16_t)(v.z - r + 1);  // round up
  int16_t z2 = (int16_t)(v.z + r);      // round down

  for (int16_t x = x1; x <= x2; x++)
    for (int16_t y = y1; y <= y2; y++)
      for (int16_t z = z1; z <= z2; z++) {
        if (((x | y | z) & 0xFFF0) == 0) {
          float radius = (Vector3(x, y, z) - v).magnitude();
          if (radius < r) {
            Display::cube[Display::cubeBuffer][x][y][z].maximize(
                c.scaled(255 * (1 - (radius / r))));
          }
        }
      }
}

// Set voxel using system coordinates
inline void radiate4(const Vector3 &v0, const Color &c, const float r) {
  Vector3 v = v0 + Vector3(CX, CY, CZ);
  int16_t x1 = (int16_t)(v.x - r + 1);  // round up
  int16_t x2 = (int16_t)(v.x + r);      // round down
  int16_t y1 = (int16_t)(v.y - r + 1);  // round up
  int16_t y2 = (int16_t)(v.y + r);      // round down
  int16_t z1 = (int16_t)(v.z - r + 1);  // round up
  int16_t z2 = (int16_t)(v.z + r);      // round down

  for (int16_t x = x1; x <= x2; x++)
    for (int16_t y = y1; y <= y2; y++)
      for (int16_t z = z1; z <= z2; z++) {
        if (((x | y | z) & 0xFFF0) == 0) {
          float radius = (Vector3(x, y, z) - v).magnitude();
          if (radius < r) {
            Display::cube[Display::cubeBuffer][x][y][z].maximize(
                c.scaled(255 / (1 + (radius * radius * radius * radius))));
          }
        }
      }
}

// Set voxel using system coordinates
inline void radiate5(const Vector3 &v0, const Color &c, const float r) {
  Vector3 v = v0 + Vector3(CX, CY, CZ);
  int16_t x1 = (int16_t)(v.x - r + 1);  // round up
  int16_t x2 = (int16_t)(v.x + r);      // round down
  int16_t y1 = (int16_t)(v.y - r + 1);  // round up
  int16_t y2 = (int16_t)(v.y + r);      // round down
  int16_t z1 = (int16_t)(v.z - r + 1);  // round up
  int16_t z2 = (int16_t)(v.z + r);      // round down

  for (int16_t x = x1; x <= x2; x++)
    for (int16_t y = y1; y <= y2; y++)
      for (int16_t z = z1; z <= z2; z++) {
        if (((x | y | z) & 0xFFF0) == 0) {
          float radius = (Vector3(x, y, z) - v).magnitude();
          if (radius < r) {
            Display::cube[Display::cubeBuffer][x][y][z].maximize(c.scaled(
                255 / (1 + (radius * radius * radius * radius * radius))));
          }
        }
      }
}

//  Draw a line through two points using system coordinates
inline void line(Vector3 a, Vector3 b) {
  // Get the difference vector.
  Vector3 n = (a - b);
  // Get the amount of steps needed
  float steps = 1 + max(abs(n.z), max(abs(n.x), abs(n.y)));
  // Get the increment vector for each dimension
  Vector3 inc = n / steps;
  // The vector is pointing in the direction of Red -> Yellow
  for (uint8_t i = 0; i <= steps; i++)
    voxel(a - (inc * i), Color(i * 8, RainbowGradientPalette));
}

//  Draw a line through two points using system coordinates
inline void line(Vector3 a, Vector3 b, Color c) {
  // Get the difference vector.
  Vector3 n = (a - b);
  // Get the amount of steps needed
  float steps = 1 + max(abs(n.z), max(abs(n.x), abs(n.y)));
  // Get the increment vector for each dimension
  Vector3 inc = n / steps;
  // The vector is pointing in the direction of Red -> Yellow
  for (uint8_t i = 0; i <= steps; i++) voxel(a - (inc * i), c);
}

//  Draw a line through two points using system coordinates
inline void line(Vector3 a, Vector3 b, Color c, const float r) {
  // Get the difference vector.
  Vector3 n = (a - b);
  // Get the amount of steps needed
  float steps = 1 + max(abs(n.z), max(abs(n.x), abs(n.y)));
  // Get the increment vector for each dimension
  Vector3 inc = n / steps;
  // The vector is pointing in the direction of Red -> Yellow
  for (uint8_t i = 0; i <= steps; i++) radiate4(a - (inc * i), c, r);
}

// Draw a line through the center of the cube with direction n
inline void line(Vector3 n) {
  // Get the normal vector n hat.
  n.normalize();
  // Determine center to corner distance
  const float scalar = sqrt(CX * CX + CY * CY + CZ * CZ);
  // Draw a line through the center
  line(n * scalar, n * -scalar);
}
#endif
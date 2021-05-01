#ifndef NOISE_H
#define NOISE_H
#include <Arduino.h>
#include <stdint.h>
/*------------------------------------------------------------------------------
 * NoiseGenerator CLASS
 *------------------------------------------------------------------------------
 * This class generates numbers according to a plan. The numbers can be random,
 * from a Perlin noise like distribution or from a Gaussian distribution.
 *
 * Perlin Noise:
 * The float x,y,z,w parameters for the noise functions use the integer part &
 * 0xff and the fractional part so the range for integer part is 0 to 255 and
 * than loops back.
 *
 * The int px, py, pz, pw parameters wrap the noise arround an integer range,
 * the range is 0 to 255. The parameters are & 0xff.
 *
 * The return float is a value between 0.0f and 1.0f arround 5% of the values
 * are clamped 2.5% at the lower end and 2.5% at the higher end.
 *
 * NOTE: NOT al values are distributed equally, and 5% of the values are clamped
 *----------------------------------------------------------------------------*/
class Noise {
 private:
  float spare;
  bool hasSpare = false;
  static const uint8_t perm[512];

 public:
  // get next normally divided value with given mean and stdev
  float nextGaussian(const float mean, const float stdev);
  // nextGaussian but with a max deviation of range * stdev
  float nextGaussian(const float mean, const float stdev, const float range);

 public:
  // get a random float value between min and max (boundaries included)
  float nextRandom(const float min, const float max) const;
  // get a random uint16_t value between min and max (boundaries included)
  uint16_t nextRandom16(const uint16_t min, const uint16_t max) const;

 private:
  float grad1(int hash, float x);
  float grad2(int hash, float x, float y);
  float grad3(int hash, float x, float y, float z);
  float grad4(int hash, float x, float y, float z, float w);

 public:
  float noise1(float x);
  float pnoise1(float x, int px);
  float noise2(float x, float y);
  float pnoise2(float x, float y, int px, int py);
  float noise3(float x, float y, float z);
  float pnoise3(float x, float y, float z, int px, int py, int pz);
  float noise4(float x, float y, float z, float w);
  float pnoise4(float x, float y, float z, float w, int px, int py, int pz,
                int pw);
};
#endif
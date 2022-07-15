#ifndef MATH8_H_
#define MATH8_H_
/*------------------------------------------------------------------------------
 * The map function maps the distance (in) on a scale of inMin to inMax to the
 * distance (out) on a scale of outMin to outMax.
 *
 * Keep range of value between 0x00 and 0x100 inclusive fiting in uint16_t. The
 * scalar is between 0x00 and 0xFF. Value * scalar still fits inside uint16_t
 *----------------------------------------------------------------------------*/
static inline uint8_t map8(uint8_t in, uint8_t inMin, uint8_t inMax,
                           uint8_t outMin, uint8_t outMax) {
  if (outMax > outMin)
    return outMin + (outMax - outMin) * (in - inMin) / (inMax - inMin);
  else
    return outMin - (outMin - outMax) * (in - inMin) / (inMax - inMin);
}
static inline uint8_t map8(uint8_t scalar, uint8_t outMin, uint8_t outMax) {
  if (outMax > outMin)
    return outMin + (((outMax - outMin + 1) * scalar) >> 8);
  else
    return outMin - (((outMin - outMax + 1) * scalar) >> 8);
}
static inline uint8_t map8(uint8_t scalar, uint8_t outMax) {
  return (((outMax + 1) * scalar) >> 8);
}
static inline float mapf(float in, float inMin, float inMax, float outMin,
                         float outMax) {
  return (outMax - outMin) * (in - inMin) / (inMax - inMin) + outMin;
}
static inline uint8_t qadd8(uint8_t a, uint8_t b) {
  uint16_t t = a + b;
  if (t > 255) return 255;
  return t;
}
static inline uint8_t qsub8(uint8_t a, uint8_t b) {
  if (b > a) return 0;
  return a - b;
}
static inline uint8_t scale8(uint8_t i, uint8_t scale) {
  return (((uint16_t)i) * (1 + (uint16_t)(scale))) >> 8;
}
// Blend A with B (amount_b range = 0 to 255)
// Blend8 diverges to 0 and to 255 but not necessarily to other values
// Scale8 isn't meant for blending but only mapping between values,
// Scale8 will always diverge if you adjust the scale, but not while
// blending without keeping source and origional colors.
static inline uint8_t blend8(uint8_t amount_b, uint8_t a, uint8_t b) {
  uint16_t partial;
  partial = (a << 8) | b;
  partial += (b * amount_b);
  partial -= (a * amount_b);
  return partial >> 8;
}
#endif
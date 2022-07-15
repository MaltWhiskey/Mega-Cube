#include "Color.h"

#include <Arduino.h>

#include "Math8.h"
/*------------------------------------------------------------------------------
 * Color CLASS
 *----------------------------------------------------------------------------*/
Color::Color() {
  red = 0;
  green = 0;
  blue = 0;
}
Color::Color(const Color& c) {
  red = c.red;
  green = c.green;
  blue = c.blue;
}
Color::Color(const uint8_t red_, const uint8_t green_, const uint8_t blue_) {
  red = red_;
  green = green_;
  blue = blue_;
}
// Create a random color with red, green and blue between min and max inclusive
Color::Color(const uint8_t min, const uint8_t max) {
  red = random(min, max + 1);
  green = random(min, max + 1);
  blue = random(min, max + 1);
}
// Create a color from a color pallete, by interpolating between entries
Color::Color(const uint8_t index, const uint8_t* pallete) {
  uint8_t i = 0;
  while (index > pallete[i]) i += 4;
  if (pallete[i] != index) {
    red = map8(index, pallete[i - 4], pallete[i], pallete[i + 1 - 4],
               pallete[i + 1]);
    green = map8(index, pallete[i - 4], pallete[i], pallete[i + 2 - 4],
                 pallete[i + 2]);
    blue = map8(index, pallete[i - 4], pallete[i], pallete[i + 3 - 4],
                pallete[i + 3]);
  } else {
    red = pallete[i + 1];
    green = pallete[i + 2];
    blue = pallete[i + 3];
  }
}
// Create a color between source and target. Scaler determine how much between
// scalar = 000 -> source
// scalar = 200 -> (55/256)% source + (200/256)% target
// scalar = 255 -> target
Color::Color(const uint8_t scalar, const Color& source, const Color& target) {
  red = map8(scalar, source.r, target.r);
  green = map8(scalar, source.g, target.g);
  blue = map8(scalar, source.b, target.b);
}
// Scales a color blend between 0x00 and 0xff. Scaler determine how much between
Color& Color::scale(const uint8_t scalar) {
  red = map8(scalar, red);
  green = map8(scalar, green);
  blue = map8(scalar, blue);
  return *this;
}
// Blends a color between source and target, diverges to 0 and 255
Color& Color::blend(const uint8_t scalar, const Color& target) {
  r = blend8(scalar, r, target.r);
  g = blend8(scalar, g, target.g);
  b = blend8(scalar, b, target.b);
  return *this;
}
// Create a color blend between 0x00 and 0xff. Scaler determine how much between
Color Color::scaled(const uint8_t scalar) const {
  return Color(map8(scalar, red), map8(scalar, green), map8(scalar, blue));
}
// Chooses maximium of two different colors
Color& Color::maximize(const Color& c) {
  r = max(red, c.red);
  g = max(green, c.green);
  b = max(blue, c.blue);
  return *this;
}
bool Color::isBlack() const { return (r | g | b) == 0; }
bool Color::operator==(const Color& c) const {
  return ((red == c.r) & (green == c.g) & (blue == c.b));
}
Color& Color::gamma() {
  red = GAMMA[red];
  green = GAMMA[green];
  blue = GAMMA[blue];
  return *this;
}
Color Color::operator+(const Color& c) const { return Color(*this) += c; }
Color Color::operator-(const Color& c) const { return Color(*this) -= c; }

Color& Color::operator+=(const Color& c) {
  uint16_t t;
  t = r + c.r;
  t > 255 ? r = 255 : r = t;
  t = g + c.g;
  t > 255 ? g = 255 : g = t;
  t = b + c.b;
  t > 255 ? b = 255 : b = t;
  return *this;
}
Color& Color::operator-=(const Color& c) {
  uint16_t t;
  t = r - c.r;
  t > 255 ? r = 0 : r = t;
  t = g - c.g;
  t > 255 ? g = 0 : g = t;
  t = b - c.b;
  t > 255 ? b = 0 : b = t;
  return *this;
}
uint32_t Color::bits() { return (red << 24) | (green << 16) | (blue << 8); }

const Color Color::BLACK(0x00, 0x00, 0x00);
const Color Color::WHITE(0xFF, 0xFF, 0xFF);
const Color Color::RED(0xFF, 0x00, 0x00);
const Color Color::GREEN(0x00, 0xFF, 0x00);
const Color Color::BLUE(0x00, 0x00, 0xFF);
const Color Color::YELLOW(0xFF, 0xFF, 0x00);
const Color Color::ORANGE(0xFF, 0x80, 0x00);
const Color Color::PINK(0x80, 0x30, 0x80);
const Color Color::CYAN(0x00, 0xFF, 0xFF);
const Color Color::MAGENTA(0xFF, 0x00, 0xFF);
const Color Color::BROWN(0x7B, 0x40, 0x13);
const uint8_t Color::GAMMA[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
    2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,
    4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,
    8,   9,   9,   9,   10,  10,  10,  11,  11,  11,  12,  12,  13,  13,  13,
    14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,
    21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,
    31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,  42,
    43,  44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  54,  55,  56,  57,
    58,  59,  60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  72,  73,  74,
    75,  77,  78,  79,  81,  82,  83,  85,  86,  87,  89,  90,  92,  93,  95,
    96,  98,  99,  101, 102, 104, 105, 107, 109, 110, 112, 114, 115, 117, 119,
    120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142, 144, 146,
    148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175, 177,
    180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252,
    255};

// Color gradient Palletes, starts with an index and than red, green, blue
// The first index starts at 0 and must increase and the last must be 255
uint8_t RainbowGradientPalette[] = {
    0,   255, 0,   0,    // Red
    32,  171, 85,  0,    // Orange
    64,  171, 171, 0,    // Yellow
    96,  0,   255, 0,    // Green
    128, 0,   171, 85,   // Aqua
    160, 0,   0,   255,  // Blue
    192, 85,  0,   171,  // Purple
    224, 171, 0,   85,   // Pink
    255, 255, 0,   0     // and back to Red
};

uint8_t SunsetRealPalette[] = {
    0,   120, 0,   0,    //
    22,  179, 22,  0,    //
    51,  255, 104, 0,    //
    85,  167, 22,  18,   //
    135, 100, 0,   103,  //
    198, 16,  0,   130,  //
    255, 0,   0,   160   //
};

uint8_t LavaPalette[] = {
    0,   0,   0,   0,   //
    46,  18,  0,   0,   //
    96,  113, 0,   0,   //
    108, 142, 3,   1,   //
    119, 175, 17,  1,   //
    146, 213, 44,  2,   //
    174, 255, 82,  4,   //
    188, 255, 115, 4,   //
    202, 255, 156, 4,   //
    218, 255, 203, 4,   //
    234, 255, 255, 4,   //
    244, 255, 255, 71,  //
    255, 255, 255, 255  //
};
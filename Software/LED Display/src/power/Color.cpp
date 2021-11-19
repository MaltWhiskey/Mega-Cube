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
// Create a color blend between source and target. Scaler determine how much
// between scaler = 000 -> (source +   0% delta) = source scaler = 255 ->
// (source + 100% delta) = target
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
// Create a color blend between 0x00 and 0xff. Scaler determine how much between
Color Color::scaled(const uint8_t scalar) const {
  return Color(map8(scalar, red), map8(scalar, green), map8(scalar, blue));
}
bool Color::isBlack() const { return (r | g | b) == 0; }
bool Color::operator==(const Color& c) const {
  return ((red == c.r) & (green == c.g) & (blue == c.b));
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

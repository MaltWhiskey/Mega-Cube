#ifndef COLOR_H
#define COLOR_H
#include <stdint.h>
/*---------------------------------------------------------------------------------------
 * Globals
 *-------------------------------------------------------------------------------------*/
extern uint8_t RainbowGradientPalette[];
extern uint8_t SunsetRealPalette[];
extern uint8_t LavaPalette[];
/*---------------------------------------------------------------------------------------
 * Color CLASS
 *---------------------------------------------------------------------------------------
 * Color had a red, blue and green value
 *
 * Different contructors are available to get the desired color. Colors can
 *be generated from a pallete, predefined values or randomly generated.
 *-------------------------------------------------------------------------------------*/
class Color {
 public:
  union {
    uint8_t r;
    uint8_t red;
  };
  union {
    uint8_t g;
    uint8_t green;
  };
  union {
    uint8_t b;
    uint8_t blue;
  };

 public:
  static const Color BLACK;
  static const Color WHITE;
  static const Color RED;
  static const Color GREEN;
  static const Color BLUE;
  static const Color YELLOW;
  static const Color ORANGE;
  static const Color PINK;
  static const Color CYAN;
  static const Color MAGENTA;
  static const Color BROWN;
  static const uint8_t GAMMA[256];

 public:
  Color();
  Color(const Color& c);
  Color(const uint8_t red, const uint8_t green, uint8_t blue);
  Color(const uint8_t min, const uint8_t max);
  Color(const uint8_t index, const uint8_t* pallete);
  Color(const uint8_t scalar, const Color& source, const Color& target);

  Color& scale(const uint8_t scalar);
  Color scaled(const uint8_t scalar) const;
  Color& blend(const uint8_t scalar, const Color& target);
  Color& maximize(const Color& c);

  Color& gamma();
  uint32_t bits();

  Color& operator+=(const Color& c);
  Color& operator-=(const Color& c);

  Color operator+(const Color& c) const;
  Color operator-(const Color& c) const;
  bool operator==(const Color& c) const;
  bool isBlack() const;
};
#endif
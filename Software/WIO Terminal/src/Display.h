#pragma once
#include <stdint.h>

class Display
{
public:
  virtual void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) = 0;
  virtual void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) = 0;
  virtual void setSwapBytes(bool swap) = 0;
  virtual void pushImage(int32_t x, int32_t y, int32_t width, int32_t height, uint16_t *pixels) = 0;
  virtual int32_t width() = 0;
  virtual int32_t height() = 0;
};

template <class T>
class DisplayWrapper : public Display
{
private:
  T &display;

public:
  DisplayWrapper(T &display) : display(display)
  {
  }
  inline void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) override
  {
    display.drawLine(x0, y0, x1, y1, color);
  }
  inline void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) override
  {
    display.fillRect(x, y, w, h, color);
  }
  inline void setSwapBytes(bool swap) override
  {
    display.setSwapBytes(swap);
  }
  inline void pushImage(int32_t x, int32_t y, int32_t width, int32_t height, uint16_t *pixels) override
  {
    display.pushImage(x, y, width, height, pixels);
  }
  inline int32_t width() override
  {
    return display.width();
  }
  inline int32_t height() override
  {
    return display.height();
  }
};
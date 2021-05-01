#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>

#include "Display.h"
#include "Math3D.h"
#include "Math8.h"
#include "Noise.h"
#include "Timer.h"
/*------------------------------------------------------------------------------
 * ANIMATION INTERFACE
 *----------------------------------------------------------------------------*/
class Animation {
 protected:
  // Reference to the display (initialized only once)
  static Display& display;
  // Position in Color Wheel, multiplied by 256 for more resolution.
  static uint16_t colorwheel;
  // Shared staticnoise object
  static Noise noise;

 private:
  // Timer used to make rendering frequency independent
  static Timer m_cTimer;
  // Total animation pointers in animation list
  static uint8_t m_iAnimations;
  // Pointers to animations in an animation list
  static Animation* m_pAnimations[100];
  // Current animation in the animation list
  static uint8_t m_iCurrentAnimation;
  // Requested animation in the animation list
  static uint8_t m_iRequestedAnimation;

 public:
  // Constructor inserts this animation in the list
  Animation();
  // virtual destructor
  virtual ~Animation(){};
  // draw method needs to be overridden
  virtual bool draw(float dt) = 0;
  // init method needs to be overridden
  virtual void init() = 0;
  // animation scheduler
  static void animate();
  // initialize animation list, first animation and display
  static void begin();

  // Cycle through the animations
  static void next();
  static void previous();
  static void set(uint8_t index);
  static uint8_t get();
};
#endif
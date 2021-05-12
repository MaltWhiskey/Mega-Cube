#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>

#include "core/Display.h"
#include "power/Math3D.h"
#include "power/Math8.h"
#include "power/Noise.h"
#include "power/Timer.h"
/*------------------------------------------------------------------------------
 * ANIMATION INTERFACE
 *----------------------------------------------------------------------------*/
enum task_t { INACTIVE = 0, STARTING = 1, RUNNING = 2, ENDING = 3 };
enum anim_t { COLOR, WHITE, NONE, FADE };

class Animation {
 private:
  // Timer used to make rendering frequency independent
  static Timer s_cTimer;
  // Total pointers in animation pool
  static uint8_t s_iAnimations;
  // Pointers to animations in pool
  static Animation* s_pAnimations[100];
  // Position in the sequence table
  static uint16_t s_iSequence;

 protected:
  // Reference to the display (initialized only once)
  static Display& display;
  // Shared noise object
  static Noise noise;
  // Postition in color palette << 8 for more resolution
  uint16_t hue16 = 0;
  // Animation is active and drawn on the display
  task_t task = INACTIVE;
  // How to start animaton, gracefully or otherwise
  anim_t start_mode = NONE;
  // How to run the animaton
  anim_t run_mode = NONE;
  // How to end animaton, gracefully or otherwise
  anim_t end_mode = NONE;

 public:
  // Adds animation to animation pool during construction
  Animation() { s_pAnimations[s_iAnimations++] = this; };
  virtual ~Animation(){};
  // Start displaying animations
  static void begin();
  // Animate all active animations
  static void animate();
  // Schedule next animations from sequence
  static void sequence();
  // Draws antimation frame respecting elapsed time
  virtual void draw(float dt) = 0;
  // Get current fps
  static float fps();
};
#endif
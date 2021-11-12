#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>

#include "Core/Config.h"
#include "core/Display.h"
#include "power/Math3D.h"
#include "power/Math8.h"
#include "power/Noise.h"
#include "power/Timer.h"
/*------------------------------------------------------------------------------
 * ANIMATION INTERFACE
 *----------------------------------------------------------------------------*/
// Task mode, mutually exclusive
enum class task_state_t { INACTIVE = 0, STARTING = 1, RUNNING = 2, ENDING = 3 };

class Animation {
 private:
  // Timer used to make rendering frequency independent
  static Timer animation_timer;
  // Position in the sequence table
  static uint16_t animation_sequence;

 protected:
  // Shared noise object
  static Noise noise;
  // Postition in color palette << 8 for more resolution
  uint16_t hue16 = 0;
  // Animation is active and drawn on the display
  task_state_t task = task_state_t::INACTIVE;

 public:
  virtual ~Animation(){};
  // Start displaying animations
  static void begin();
  // Animate all active animations
  static void animate();
  // Schedule next animations from sequence
  static void next();
  // select animations from sequence
  static void select();
  // Get current fps
  static float fps();
  // Draws antimation frame respecting elapsed time
  virtual void draw(float dt) = 0;
  // Gracefully terminate animation
  void end();
};
#endif
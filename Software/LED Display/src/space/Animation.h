#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>

#include "core/Config.h"
#include "core/Display.h"
#include "core/Graphics.h"
#include "power/Math3D.h"
#include "power/Math8.h"
#include "power/Noise.h"
#include "power/Particle.h"
#include "power/Timer.h"
/*------------------------------------------------------------------------------
 * ANIMATION INTERFACE
 *----------------------------------------------------------------------------*/
// Task mode, mutually exclusive
enum class state_t : uint8_t {
  INACTIVE = 0,
  STARTING = 1,
  RUNNING = 2,
  ENDING = 3
};

class Animation {
 private:
  // Timer used to make rendering frequency independent
  static Timer animation_timer;
  // Position in the sequence table
  static uint16_t animation_sequence;

 public:
  // Shared noise object
  static Noise noise;
  // Postition in color palette << 8 for more resolution
  uint16_t hue16 = 0;
  // Animation is active and drawn on the display
  state_t state = state_t::INACTIVE;

 public:
  virtual ~Animation(){};
  // Start displaying animations
  static void begin();
  // Animate all active animations
  static void animate();
  // Select next animation from sequence or a specific one
  static void next(boolean endless = false, uint8_t index = 0);
  // Get current fps
  static float fps();
  // Draws antimation frame respecting elapsed time
  virtual void draw(float dt) = 0;
  // End animation gracefully
  virtual void end();
};
#endif
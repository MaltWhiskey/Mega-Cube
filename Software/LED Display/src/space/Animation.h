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

class Animation;

// Jump item configuration
struct jump_item_t {
  const char* name;
  const char* description;
  void (*custom_init)();
  Animation* object;
};

class Animation {
 private:
  // Timer used to make rendering frequency independent
  static Timer animation_timer;
  // Position in the sequence table
  static uint16_t animation_sequence;
  // Settings of animations
  static constexpr auto& settings = config.animation;

 public:
  // Shared Noise Object
  static Noise noise;
  // Amount of (aprox) time this animation keeps running
  Timer timer_starting;
  Timer timer_running;
  Timer timer_ending;
  // Postition in color palette << 8 for more resolution
  int16_t hue16;
  int16_t hue16_speed;
  // Animation brightness
  uint8_t brightness;
  // When animation is not inactive draw this on the display
  state_t state = state_t::INACTIVE;
  // When animation is ended, time is reduced only once
  bool time_reduction = false;

  virtual ~Animation(){};
  // Start displaying animations
  static void begin();
  // Animate all active animations
  static void loop();
  // Select next animation from sequence or a specific one
  static void next(boolean changed = false, uint16_t index = 0);
  // Get current fps
  static float fps();
  // Draws antimation frame respecting elapsed time
  virtual void draw(float dt) = 0;
  // End animation gracefully (may be overriden)
  virtual void end();
  // Initialization of animation with configuration
  virtual void init() = 0;
  // Get animation jump_items_t (returns 0 when above index)
  static jump_item_t get_item(uint16_t index);
};

inline void setMotionBlur(uint8_t n) { Display::setMotionBlur(n); }
inline uint8_t getMotionBlur() { return Display::getMotionBlur(); }
inline void setBrightness(float n) { Display::setBrightness(n); }
inline float getBrightness() { return Display::getBrightness(); }
#endif
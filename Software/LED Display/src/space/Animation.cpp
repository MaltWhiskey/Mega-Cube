#include "Animation.h"

#include "Arrows.h"
#include "Atoms.h"
#include "Fireworks.h"
#include "Helix.h"
#include "Plasma.h"
#include "Sinus.h"
#include "Starfield.h"
#include "Twinkels.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
Noise Animation::noise = Noise();
Timer Animation::animation_timer = Timer();
uint16_t Animation::animation_sequence = 0;
/*------------------------------------------------------------------------------
 * ANIMATION GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/
Sinus sinus;
Fireworks fireworks1;
Fireworks fireworks2;
Twinkels twinkels;
Starfield starfield;
Helix helix;
Atoms atoms;
Arrows arrows;
Plasma plasma;
Animation *Animations[] = {&atoms,      &sinus,      &starfield,
                           &fireworks1, &fireworks2, &twinkels,
                           &helix,      &arrows,     &plasma};
const uint8_t ANIMATIONS = sizeof(Animations) / sizeof(Animation *);
/*----------------------------------------------------------------------------*/
// Start display asap to minimize PL9823 blue startup
void Animation::begin() { Display::begin(); }
// Gracefully terminate animation
void Animation::end() { task = task_state_t::ENDING; }

// Render an animation frame, but only if the display allows it (non blocking)
void Animation::animate() {
  // Only draw to the display if it's available
  if (Display::available()) {
    // Clear the display before drawing any animations
    Display::clear();
    // Update the animation timer to determine frame deltatime
    animation_timer.update();
    // Draw all active animations from the animation pool
    uint8_t active_count = 0;
    for (uint8_t i = 0; i < ANIMATIONS; i++) {
      Animation &animation = *Animations[i];
      if (animation.task != task_state_t::INACTIVE) {
        animation.draw(animation_timer.dt());
      }
      if (animation.task != task_state_t::INACTIVE) {
        active_count++;
        if (config.animation.animation < ANIMATIONS) {
          animation.end();
        }
      }
    }
    if (active_count == 0) {
      if (config.animation.animation < ANIMATIONS) {
        Animation::select();
      } else {
        Animation::next();
      }
    }
    // Commit current animation frame to the display
    Display::update();
  }
}

// Get fps, if animate has been called t > 0
float Animation::fps() {
  if (animation_timer.dt() > 0) {
    return 1 / animation_timer.dt();
  }
  return 0;
}

void SEQ_TWINKEL_WHITE_00(void) {
  twinkels.init(config.twinkels.timer_duration, true, false);
  twinkels.speed(config.twinkels.timer_interval, config.twinkels.fade_in_speed,
                 config.twinkels.fade_out_speed);
  twinkels.color(Color(255, 150, 30));
  twinkels.clear();
}
void SEQ_TWINKEL_MULTI_00(void) {
  twinkels.init(config.twinkels.timer_duration, false, true);
  twinkels.speed(config.twinkels.timer_interval, config.twinkels.fade_in_speed,
                 config.twinkels.fade_out_speed);
}
void SEQ_SINUS_00(void) {
  sinus.init(5, 20, 10);
  sinus.speed(PI, -50);
}
void SEQ_FIREWORKS_00(void) {
  fireworks1.init(5);
  fireworks2.init(5);
}
void SEQ_STARFIELD_00(void) {
  starfield.init(config.starfield.fade_in_speed,
                 config.starfield.timer_duration,
                 config.starfield.fade_out_speed);
  starfield.speed(config.starfield.phase_speed, config.starfield.hue_speed);
}
void SEQ_HELIX_00(void) { helix.init(); }
void SEQ_ATOMS_00(void) { atoms.init(); }
void SEQ_ARROWS_00(void) { arrows.init(); }
void SEQ_PLASMA_00(void) { plasma.init(); }

// Animation sequencer jumptable implementation
void Animation::next() {
  static void (*jump_table[])() =  //
      {&SEQ_PLASMA_00, &SEQ_ARROWS_00,        &SEQ_ATOMS_00,
       &SEQ_HELIX_00,  &SEQ_STARFIELD_00,     &SEQ_FIREWORKS_00,
       &SEQ_SINUS_00,  &SEQ_TWINKEL_WHITE_00, &SEQ_TWINKEL_MULTI_00};
  if (animation_sequence >= sizeof(jump_table) / sizeof(void *)) {
    animation_sequence = 0;
  }
  jump_table[animation_sequence++]();
}

// Animation selector jumptable implementation
void Animation::select() {
  static void (*jump_table[])() =  //
      {&SEQ_FIREWORKS_00, &SEQ_SINUS_00, &SEQ_TWINKEL_WHITE_00,
       &SEQ_TWINKEL_MULTI_00};
  if (config.animation.animation >= sizeof(jump_table) / sizeof(void *)) {
    Animation::next();
  } else {
    jump_table[config.animation.animation]();
  }
  config.animation.animation = 0xff;
}
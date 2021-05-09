#include "Animation.h"

#include "fireworks.h"
#include "sinus.h"
#include "twinkels.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
Display &Animation::display = Display::instance();
Timer Animation::s_cTimer = Timer();
Noise Animation::noise = Noise();
uint8_t Animation::s_iAnimations = 0;
Animation *Animation::s_pAnimations[100];
uint16_t Animation::s_iSequence = 0;
/*------------------------------------------------------------------------------
 * ANIMATION GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/
Sinus sinus;
Fireworks fireworks1;
Fireworks fireworks2;
Twinkels twinkels;
/*----------------------------------------------------------------------------*/
// Start display asap to minimize PL9823 blue startup
void Animation::begin() { display.begin(); }

// Render an animation frame, but only if the display allows it
void Animation::animate() {
  // Only draw to the display if it's available
  if (display.available()) {
    // Clear the display before drawing any animations
    display.clear();
    // Update the animation timer to determine frame deltatime
    s_cTimer.update();
    // draw all active animations from the animation pool
    uint8_t active_count = 0;
    for (uint8_t i = 0; i < s_iAnimations; i++) {
      Animation &animation = *s_pAnimations[i];
      if (animation.task != INACTIVE) animation.draw(s_cTimer.dt());
      if (animation.task != INACTIVE) active_count++;
    }
    if (active_count == 0) sequence();
    // Commit current animation frame to the display
    display.update();
  }
}

void SEQ_TWINKEL_WHITE_00(void) {
  twinkels.init(20, NONE, WHITE, NONE);
  twinkels.speed(0.1f, 1.0f, 2.0f);
  twinkels.clear();
}
void SEQ_TWINKEL_MULTI_00(void) {
  twinkels.init(20, NONE, COLOR, FADE);
  twinkels.speed(0.1f, 1.0f, 2.0f);
}
void SEQ_SINUS_00(void) {
  sinus.init(5, 20, 10);
  sinus.speed(PI, -50);
}
void SEQ_FIREWORKS_00(void) {
  fireworks1.init(5);
  fireworks2.init(5);
}

// Animation sequencer jumptable implementation
void Animation::sequence() {
  static void (*jump_table[])() =  //
      {&SEQ_FIREWORKS_00, &SEQ_SINUS_00, &SEQ_TWINKEL_WHITE_00,
       &SEQ_TWINKEL_MULTI_00};
  if (s_iSequence >= sizeof(jump_table) / sizeof(void *)) {
    s_iSequence = 0;
  }
  jump_table[s_iSequence++]();
}
#include "Animation.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
// Position in the color wheel, multiplied by 256 for more resolution.
uint16_t Animation::colorwheel = 0;
// Reference to the display (initialized only once)
Display &Animation::display = Display::instance();
// Timer used to make rendering frequency independent
Timer Animation::m_cTimer = Timer();
// Noise object shared between all subclasses
Noise Animation::noise = Noise();
// Total animation pointers in animation list
uint8_t Animation::m_iAnimations = 0;
// Current animation in the animation list
uint8_t Animation::m_iCurrentAnimation = 0;
// Requested animation in the animation list
uint8_t Animation::m_iRequestedAnimation = 0;
// Pointers to animations in a animation list max 100
Animation *Animation::m_pAnimations[100];

// Add this animation to animation list
Animation::Animation() { m_pAnimations[m_iAnimations++] = this; }

// Allows cycling through the animations
void Animation::next() {
  uint8_t requested = m_iCurrentAnimation + 1;
  if (requested >= m_iAnimations) requested = 0;
  m_iRequestedAnimation = requested;
}

// Allows cycling through the animations
void Animation::previous() {
  uint8_t requested = m_iCurrentAnimation - 1;
  if (requested == 0xff) requested = m_iAnimations - 1;
  m_iRequestedAnimation = requested;
}

// Allows selecting a specific animation
void Animation::set(uint8_t requested) {
  uint8_t current = m_iCurrentAnimation;
  if (requested != current && requested < m_iAnimations) {
    m_iRequestedAnimation = requested;
  }
}

// Current animation
uint8_t Animation::get() { return m_iCurrentAnimation; }

// Render an animation frame, but only if the display allows it
void Animation::animate() {
  // Only draw to the display if it's available
  if (display.available()) {
    // These temp variable should make for a thread safe method
    uint8_t current = m_iCurrentAnimation;
    uint8_t requested = m_iRequestedAnimation;
    // Update the animation timer and get deltatime between previous frame
    m_cTimer.update();
    // if there are animations present and no out of bounds on current
    if (m_iAnimations > 0 && current < m_iAnimations) {
      // draw one animation frame using the deltatime and the display dimensions
      if (m_pAnimations[current]->draw(m_cTimer.dt())) {
        // If the animation is finished reinitialize it
        m_pAnimations[current]->init();
        Animation::next();
      }
      // Check if new animation needs to be initialized and displayed
      if (current != requested && requested < m_iAnimations) {
        m_pAnimations[requested]->init();
        m_iCurrentAnimation = requested;
      }
      // Commit current animation frame to the display
      display.update();
    }
  }
}

void Animation::begin() {
  // if animations are present, initialize the current one
  if (m_iAnimations > 0) {
    m_pAnimations[m_iCurrentAnimation]->init();
  }
  // Initialize the display
  display.begin();
}
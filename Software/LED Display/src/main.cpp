#include <Arduino.h>

#include "animation.h"
#include "fireworks.h"
#include "sinus.h"
#include "twinkels.h"
/*------------------------------------------------------------------------------
 * Global animations. The animation constructor adds these to a list
 *----------------------------------------------------------------------------*/
Sinus sinus;
Fireworks fireworks;
Twinkels twinkel;
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  Animation::begin();
  Serial.begin(115200);
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() { Animation::animate(); }
// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: SquareLine_Project

#include "space/Animation.h"
#include "ui.h"

void BTNCLICK(lv_event_t* e) {
  // Your code here
}

void EVENT_ARROWS(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.animation = 9;
  jump_item_t jump = Animation::get_item(9);
  Serial.println(jump.name);
}

void EVENT_FIREWORKS(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.animation = 4;
}

void EVENT_ATOMS(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.animation = 8;
}

void EVENT_SLIDER_CHG(lv_event_t* e) {
  // Your code here
}

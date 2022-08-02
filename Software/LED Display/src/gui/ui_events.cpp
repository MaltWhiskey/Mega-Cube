// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: LEDCUBELCD

#include "core/Config.h"
#include "ui.h"

void Event_Play_All(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = false;
}

void Event_Play_Accelerometer(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 0;
}

void Event_Play_Arrows(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 1;
}

void Event_Play_Atoms(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 2;
}

void Event_Play_Fireworks(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 3;
}

void Event_Play_Helix(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 4;
}

void Event_Play_Life(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 5;
}

void Event_Play_Mario(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 6;
}

void Event_Play_Plasma(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 7;
}

void Event_Play_Pong(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 8;
}

void Event_Play_Scroller(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 9;
}

void Event_Play_Sinus(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 10;
}

void Event_Play_Spectrum(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 11;
}

void Event_Play_Starfield(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 12;
}

void Event_Play_Twinkels1(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 13;
}

void Event_Play_Twinkels2(lv_event_t* e) {
  config.animation.changed = true;
  config.animation.play_one = true;
  config.animation.animation = 14;
}
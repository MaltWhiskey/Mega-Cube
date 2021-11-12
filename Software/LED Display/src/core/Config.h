#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <stdint.h>
/*-----------------------------------------------------------------------------
 * Evil global Config parameters
 *
 * Animation init or draw routines need to apply config parameters to
 * dynamically set runtime parameters. Init only gets called when an animation
 * starts or restarts draw gets called every animation frame so choose wisely
 * where to apply.
 *---------------------------------------------------------------------------*/
struct Config {
  struct {
    char ssid[32] = "-^..^-";
    char password[64] = "qazwsxedc";
    char hostname[64] = "MegaCube";
  } network;
  struct {
    uint8_t animation = 0xff;
  } animation;
  struct {
    float timer_duration = 20.0f;
    float timer_interval = 0.1f;
    float fade_in_speed = 1.0f;
    float fade_out_speed = 2.0f;
  } twinkels;
  struct {
    float timer_duration = 20.0f;
    float fade_in_speed = 3.0f;
    float fade_out_speed = 5.0f;
    float phase_speed = 0.3f;
    uint16_t hue_speed = 30;
  } starfield;
  struct {
    float timer_duration = 20.0f;
    float fade_in_speed = 3.0f;
    float fade_out_speed = 5.0f;
    float phase_speed = 3.0f;
    uint16_t hue_speed = 30;
  } spin;
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif
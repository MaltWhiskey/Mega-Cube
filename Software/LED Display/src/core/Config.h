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
  struct {
    float timer_duration = 60.0f;
    uint8_t brightness = 70;
    // scale_p represents the distance between each pixel in the noise map
    float scale_p = 0.15f;
    // speed is how fast the movement is over the axis in the noise map
    float speed_x = 0.1f;
    float speed_y = 0.2f;
    float speed_z = 0.3f;
    float speed_w = 0.4f;
    // speed_offset_speed is the travel distance through a 1d noise map
    float speed_offset_speed = 0.02f;
    // The speed for changing the hue values
    uint16_t hue_speed = 30;
  } noise;
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif
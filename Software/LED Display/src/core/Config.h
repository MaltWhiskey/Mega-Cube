#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <stdint.h>
/*-----------------------------------------------------------------------------
 * Evil global parameters
 *
 * These parameters are used for dynamically changing runtime operation. They
 * can (optionally) be loaded from and writen to persistant storage.
 *
 * Animation init or draw routines need to apply these parameters to
 * dynamically set runtime parameters. Init only gets called when an animation
 * starts or restarts draw gets called every animation frame so choose wisely
 * where to apply. And let these parameters take effect.
 *---------------------------------------------------------------------------*/
struct Config {
  struct {
    struct {
      char ssid[32] = "-^..^-";
      char password[64] = "qazwsxedc";
    } wifi;
    struct {
      char hostname[64] = "MegaCube";
      uint16_t port = 8080;
    } server;
  } network;
  struct {
    boolean changed = false;
    boolean play_one = false;
    uint8_t animation = 0;
    struct {
      float runtime = 30.0f;
      uint8_t motionBlur = 220;
    } accelerometer;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 3.0f;
      float angle_speed = -180.0f;
      float radius = 7.5f;
      float radius_start = 2.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 200;
      uint8_t motionBlur = 220;
    } arrows;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 3.0f;
      float angle_speed = 120.0f;
      float radius = 7.5f;
      float radius_start = 0.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 200;
      uint8_t motionBlur = 200;
    } atoms;
    struct {
      float runtime = 30.0f;
      float radius = 7.5f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 220;
    } fireworks;
    struct {
      float runtime = 30.0f;
      float interval = 0.2f;
      float phase_speed = 5.0f;
      float angle = 0.0f;
      float angle_speed = 0.0f;
      float radius = 7.5f;
      float resolution = 30;
      int8_t hue_speed = 30;
      uint8_t brightness = 255;
      uint8_t motionBlur = 230;
    } helix;
    struct {
      float runtime = 30.0f;
      float interval = 0.20f;
      uint8_t brightness = 200;
      uint8_t motionBlur = 0;
    } life;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 3.0f;
      float interval = 0.15f;
      float angle_speed = -70.0f;
      float radius = 7.5f;
      float radius_start = 0.0f;
      uint8_t brightness = 200;
      uint8_t motionBlur = 220;
    } mario;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 10.0f;
      float scale_p = 0.15f;
      float speed_x = 0.1f;
      float speed_y = 0.2f;
      float speed_z = 0.3f;
      float speed_w = 0.4f;
      float speed_offset_speed = 0.02f;
      int8_t hue_speed = 30;
      uint8_t motionBlur = 0;
      uint8_t brightness = 70;
    } plasma;
    struct {
      float starttime = 2.0f;
      float runtime = 30.0f;
      float endtime = 2.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 200;
      uint8_t motionBlur = 200;
    } pong;
    struct {
      float runtime = 30.0f;
      float rotation_speed = 90.0f;
      uint8_t brightness = 200;
      uint8_t motionBlur = 220;
    } scroller;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 10.0f;
      float phase_speed = PI;
      float resolution = 30;
      float radius = 7.5f;
      int8_t hue_speed = -50;
      uint8_t brightness = 200;
      uint8_t motionBlur = 0;
    } sinus;
    struct {
      float runtime = 30.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 200;
      uint8_t motionBlur = 220;
    } spectrum;
    struct {
      float starttime = 3.0f;
      float runtime = 30.0f;
      float endtime = 5.0f;
      float phase_speed = 0.3f;
      int8_t hue_speed = 30;
      uint8_t brightness = 200;
      uint8_t motionBlur = 230;
    } starfield;
    struct {
      float runtime = 30.0f;
      float interval = 0.1f;
      float fade_in_speed = 1.0f;
      float fade_out_speed = 2.0f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } twinkels;
  } animation;
  struct {
    struct {
      int8_t x = 0;
      int8_t y = 0;
      boolean z = false;
      boolean a = false;
      boolean b = false;
      boolean c = false;
    } button;
    struct {
      float x = 0;
      float y = 0;
      float z = 0;
    } accelerometer;
    struct {
      uint8_t data[64];
      boolean updated = false;
    } fft;
  } hid;
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif
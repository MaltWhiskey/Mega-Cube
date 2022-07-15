#ifndef SCROLLER_H
#define SCROLLER_H

#include "Animation.h"
#include "gfx/Charset.h"
#include "power/Math8.h"

class Scroller : public Animation {
 private:
  Timer timer_duration;
  float text_rotation;
  float text_rotation_speed;
  String txt;

 public:
  void init(float duration, float speed, String text) {
    state = state_t::RUNNING;
    timer_duration = duration;
    text_rotation_speed = speed;
    text_rotation = -95.0f;
    txt = text;
  }

  void draw(float dt) {
    setMotionBlur(config.animation.scroller.motionBlur);
    if (timer_duration.update()) {
      state = state_t::ENDING;
    }

    // Amount of degrees the text has been rotated
    text_rotation += text_rotation_speed * dt;
    // Full circle resolution (radius 15) in amount of pixels
    // ø = (sin)^-1*(1/15) -> 360 / 2 * ø ≈ 47 for radius 7.5
    // for radius 15 -> 47 * 2 ≈ 94. (changes height of charset)
    float circle_resolution = 94.16f;
    // Angle adjustment in degrees for each line
    float line_angle_adj = 360 / circle_resolution;
    // Amount of pixels the text has been rotated
    float pixel_start = text_rotation / line_angle_adj;
    // Start of the line_angle at a bit over 90 degrees
    float line_angle = 90.01f;
    // Fine adjustment for angles between lines.
    // Ignore because this will add a lot of jittering.
    // line_angle += fmod(text_rotation, line_angle_adj);

    // Adjust for negative rotation (inserts blanks at begin text)
    int16_t pixel_line = pixel_start;
    if (pixel_line < 0) {
      line_angle += line_angle_adj * pixel_line;
      pixel_line = 0;
    }
    //  Insert an amount of blank_lines after every character
    const uint16_t blank_lines = 1;

    uint16_t text_lines = (CHARSET_FRAME_HEIGHT + blank_lines) * txt.length();
    while (line_angle > -line_angle_adj) {
      uint16_t text_offset = pixel_line % text_lines;
      uint16_t t =
          match_char(txt[text_offset / (CHARSET_FRAME_HEIGHT + blank_lines)]);
      if (t == 0xffff) {
        if (line_angle > 90.0f) {
          text_rotation = -95.0f;
          if (state == state_t::ENDING) {
            state = state_t::INACTIVE;
          }
        }
        break;
      }
      uint16_t y = pixel_line++ % (CHARSET_FRAME_HEIGHT + blank_lines);
      if (y < CHARSET_FRAME_HEIGHT) {
        Quaternion q = Quaternion(line_angle, Vector3::X);
        for (uint8_t x = 0; x < CHARSET_FRAME_WIDTH; x++) {
          uint32_t data = charset_data[t][y * CHARSET_FRAME_WIDTH + x];
          if (data & 0xff000000) {
            Color c = Color(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
            // c = Color((hue16 >> 8) + y * 3 + x * 4, RainbowGradientPalette);
            Vector3 pixel = q.rotate(Vector3(x / 15.0f, 0, -1) * 15.0f);
            // radiate(pixel, c, 1.0f, Vector3(0, 0, 15));
            voxel(pixel, c.gamma(), Vector3(-7.5, -7.5, +7.5));
          }
        }
      }
      line_angle -= line_angle_adj;
    }
    hue16 += (int16_t)(255 * 20 * dt);
  }

  uint16_t match_char(uint16_t chr) {
    if (chr >= ' ' && chr <= 'Z') return chr - ' ';
    return 0xffff;
    /*
    char characters[] = "1234567890ABCDEFGHIJKLMNOPQRTSUVWXYZ ?'\"abcde";
    for (uint16_t i = 0; i < sizeof(characters); i++)
      if (chr == characters[i]) return i;
    return 0xffff;
    */
    /*
    char characters[] = "!\"'(),.1234:;?ABCDEFGHIJKLMNOPQRTSUVWXYZ";
    for (uint16_t i = 0; i < sizeof(characters); i++)
      if (chr == characters[i]) return i;
    return 0xffff;*/
    /*
    char characters[] = "ABCDEFGHIJKLMNOPQRTSUVWXYZ";
    for (uint16_t i = 0; i < sizeof(characters); i++)
      if (chr == characters[i]) return i;
    return 0xffff;
    */
  }
};
#endif
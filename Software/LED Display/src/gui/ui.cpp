#include "ui.h"

#include <Arduino.h>

#include "core/Config.h"
#include "core/Display.h"
#include "space/Animation.h"
#include "ui_helpers.h"

///////////////////// PROTOTYPES ///////////////////
lv_obj_t* ui_create_screen();
lv_obj_t* ui_create_label(lv_obj_t* screen, uint16_t x, uint16_t y,
                          const char* text);
lv_obj_t* ui_create_slider(lv_obj_t* screen, uint16_t x, uint16_t y);
void ui_create_listed_icon(lv_obj_t* screen, uint32_t id, uint16_t x,
                           uint16_t y, lv_event_cb_t event);
void ui_create_icon(lv_obj_t* screen, uint32_t id, uint16_t x, uint16_t y,
                    lv_event_cb_t event, const lv_img_dsc_t* image,
                    const char* text);
void ui_event(lv_event_t* e);
///////////////////// VARIABLES ////////////////////
lv_obj_t* ui_animation_screens[4];
lv_obj_t* ui_configuration_screens[14];
lv_obj_t* ui_settings_screens[1];
lv_obj_t* ui_label_brightness;
lv_obj_t* ui_slider_brightness;
lv_obj_t* ui_label_motionblur;
lv_obj_t* ui_slider_motionblur;

void ui_refresh(lv_timer_t* timer) {
  lv_label_set_text_fmt(ui_label_brightness, "Brightness : %d",
                        Display::getBrightness());
  lv_slider_set_value(ui_slider_brightness, Display::getBrightness(),
                      LV_ANIM_OFF);
  lv_label_set_text_fmt(ui_label_motionblur, "Motionblur : %d",
                        Display::getMotionBlur());
  lv_slider_set_value(ui_slider_motionblur, Display::getMotionBlur(),
                      LV_ANIM_OFF);
}

void ui_init(void) {
  lv_disp_t* dispp = lv_disp_get_default();
  lv_theme_t* theme = lv_theme_basic_init(dispp);
  lv_disp_set_theme(dispp, theme);

  lv_obj_t* screen = ui_create_screen();
  ui_animation_screens[0] = screen;
  ui_create_icon(screen, 200, 5, 4, ui_event, &ui_img_settings_80_png,
                 "Play all");
  ui_create_listed_icon(screen, 0, 110, 4, ui_event);
  ui_create_listed_icon(screen, 1, 215, 4, ui_event);
  ui_create_listed_icon(screen, 2, 5, 122, ui_event);
  ui_create_listed_icon(screen, 3, 110, 122, ui_event);
  ui_create_icon(screen, 101, 215, 122, ui_event, &ui_img_next_80_png,
                 "Next page");

  screen = ui_create_screen();
  ui_animation_screens[1] = screen;
  ui_create_listed_icon(screen, 4, 5, 4, ui_event);
  ui_create_listed_icon(screen, 5, 110, 4, ui_event);
  ui_create_listed_icon(screen, 6, 215, 4, ui_event);
  ui_create_listed_icon(screen, 7, 5, 122, ui_event);
  ui_create_listed_icon(screen, 8, 110, 122, ui_event);
  ui_create_icon(screen, 102, 215, 122, ui_event, &ui_img_next_80_png,
                 "Next page");

  screen = ui_create_screen();
  ui_animation_screens[2] = screen;
  ui_create_listed_icon(screen, 9, 5, 4, ui_event);
  ui_create_listed_icon(screen, 10, 110, 4, ui_event);
  ui_create_listed_icon(screen, 11, 215, 4, ui_event);
  ui_create_listed_icon(screen, 12, 5, 122, ui_event);
  ui_create_listed_icon(screen, 13, 110, 122, ui_event);
  ui_create_icon(screen, 103, 215, 122, ui_event, &ui_img_next_80_png,
                 "Next page");

  screen = ui_create_screen();
  ui_animation_screens[3] = screen;
  ui_create_listed_icon(screen, 14, 5, 4, ui_event);
  ui_create_icon(screen, 100, 215, 122, ui_event, &ui_img_next_80_png,
                 "Next page");

  screen = ui_create_screen();
  ui_configuration_screens[0] = screen;
  lv_obj_add_event_cb(screen, ui_event, LV_EVENT_ALL, (void*)300);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_img_src(screen, &ui_img_maltwhiskey_png,
                              LV_PART_MAIN | LV_STATE_DEFAULT);

  screen = ui_create_screen();
  ui_settings_screens[0] = screen;
  lv_obj_add_event_cb(screen, ui_event, LV_EVENT_ALL, (void*)300);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui_label_brightness = ui_create_label(screen, 0, 40, "Brightness");
  ui_slider_brightness = ui_create_slider(screen, 0, 40 + 25);
  lv_obj_add_event_cb(ui_slider_brightness, ui_event, LV_EVENT_ALL, (void*)400);

  ui_label_motionblur = ui_create_label(screen, 0, 130, "Motionblur");
  ui_slider_motionblur = ui_create_slider(screen, 0, 130 + 25);
  lv_obj_add_event_cb(ui_slider_motionblur, ui_event, LV_EVENT_ALL, (void*)401);

  lv_disp_load_scr(ui_animation_screens[0]);
  lv_timer_create(ui_refresh, 500, NULL);
}

void ui_create_listed_icon(lv_obj_t* screen, uint32_t id, uint16_t x,
                           uint16_t y, lv_event_cb_t event) {
  // This must match with ID from jump_item_t
  const lv_img_dsc_t* images[] = {
      &ui_img_accelerometer_80_png, &ui_img_arrows_80_png,
      &ui_img_atoms_80_png,         &ui_img_fireworks_80_png,
      &ui_img_helix_80_png,         &ui_img_life_80_png,
      &ui_img_mario_80_png,         &ui_img_plasma_80_png,
      &ui_img_pong_80_png,          &ui_img_scroller_80_png,
      &ui_img_sinus_80_png,         &ui_img_spectrum_80_png,
      &ui_img_starfield_80_png,     &ui_img_twinkels1_80_png,
      &ui_img_twinkels2_80_png};
  const lv_img_dsc_t* image = images[id];
  const char* text = Animation::get_item(id).name;
  ui_create_icon(screen, id, x, y, event, image, text);
}

void ui_event(lv_event_t* e) {
  lv_event_code_t event = lv_event_get_code(e);
  uint32_t id = (uint32_t)lv_event_get_user_data(e);
  lv_obj_t* target = lv_event_get_target(e);
  uint32_t type = id / 100;
  uint32_t index = id % 100;
  if (event == LV_EVENT_CLICKED) {
    switch (type) {
      case 0:  // Change animation
        config.animation.changed = true;
        config.animation.play_one = true;
        config.animation.animation = index;
        break;
      case 1:  // Next Page
        _ui_screen_change(ui_animation_screens[index],
                          LV_SCR_LOAD_ANIM_MOVE_LEFT, 1000, 0);
        break;
      case 2:  // Play All
        config.animation.changed = true;
        config.animation.play_one = false;
        break;
    }
  }
  if (event == LV_EVENT_LONG_PRESSED) {
    switch (type) {
      case 0:  // Animation configuration screen (one for each animation)
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(ui_configuration_screens[index - index],
                          LV_SCR_LOAD_ANIM_FADE_ON, 1000, 0);
        break;
      case 2:  // Settings screen
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(ui_settings_screens[0], LV_SCR_LOAD_ANIM_FADE_ON,
                          1000, 0);
        break;
      case 3:  // Return from settings screen
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(ui_animation_screens[0], LV_SCR_LOAD_ANIM_FADE_ON,
                          1000, 0);
        break;
    }
  }
  if (event == LV_EVENT_VALUE_CHANGED) {
    switch (type) {
      case 4:
        int32_t value = lv_slider_get_value(target);
        if (index == 0) Display::setBrightness(value);
        if (index == 1) Display::setMotionBlur(value);
        break;
    }
  }
}

lv_obj_t* ui_create_screen() {
  lv_obj_t* screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  return screen;
}

void ui_create_icon(lv_obj_t* screen, uint32_t id, uint16_t x, uint16_t y,
                    lv_event_cb_t event, const lv_img_dsc_t* image,
                    const char* text) {
  lv_obj_t* panel = lv_obj_create(screen);
  lv_obj_set_width(panel, 100);
  lv_obj_set_height(panel, 114);
  lv_obj_set_x(panel, x);
  lv_obj_set_y(panel, y);
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_t* button = lv_btn_create(panel);
  lv_obj_set_width(button, 80);
  lv_obj_set_height(button, 80);
  lv_obj_set_x(button, 0);
  lv_obj_set_y(button, 6);
  lv_obj_set_align(button, LV_ALIGN_TOP_MID);
  // The pointer IS the value! To enable casting "id" needs to be sizeof(void*)
  // This hack enables freeing the memory for "id". No need to keep in global
  lv_obj_add_event_cb(button, event, LV_EVENT_ALL, (void*)id);
  lv_obj_set_style_radius(button, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x000000),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_clip_corner(button, true, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_img_src(button, image, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_img_opa(button, 100, LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_shadow_color(button, lv_color_hex(0xFFFFFF),
                                LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_shadow_opa(button, 255, LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_shadow_width(button, 20, LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_shadow_spread(button, 3, LV_PART_MAIN | LV_STATE_PRESSED);

  lv_obj_t* label = lv_label_create(panel);
  lv_obj_set_width(label, 100);
  lv_obj_set_height(label, 20);
  lv_obj_set_x(label, 0);
  lv_obj_set_y(label, 0);
  lv_obj_set_align(label, LV_ALIGN_BOTTOM_MID);
  lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
}

lv_obj_t* ui_create_label(lv_obj_t* screen, uint16_t x, uint16_t y,
                          const char* text) {
  lv_obj_t* label = lv_label_create(screen);
  lv_obj_set_width(label, LV_SIZE_CONTENT);
  lv_obj_set_height(label, LV_SIZE_CONTENT);
  lv_obj_set_x(label, x);
  lv_obj_set_y(label, y);
  lv_obj_set_align(label, LV_ALIGN_TOP_MID);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  return label;
}

lv_obj_t* ui_create_slider(lv_obj_t* screen, uint16_t x, uint16_t y) {
  lv_obj_t* slider = lv_slider_create(screen);
  lv_slider_set_range(slider, 0, 255);
  lv_slider_set_value(slider, 255, LV_ANIM_OFF);
  lv_obj_set_width(slider, 220);
  lv_obj_set_height(slider, 25);
  lv_obj_set_x(slider, x);
  lv_obj_set_y(slider, y);
  lv_obj_set_align(slider, LV_ALIGN_TOP_MID);

  lv_obj_set_style_radius(slider, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(slider, lv_color_hex(0x000000),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(slider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(slider, lv_color_hex(0x000000),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_opa(slider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(slider, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_color(slider, lv_color_hex(0x808080),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(slider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(slider, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(slider, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_style_radius(slider, 25, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFFFFF),
                            LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(slider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(slider, lv_color_hex(0x000000),
                                LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_border_opa(slider, 255,
                              LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(slider, 1,
                                LV_PART_INDICATOR | LV_STATE_DEFAULT);

  lv_obj_set_style_radius(slider, 20, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(slider, lv_color_hex(0x808080),
                            LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(slider, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_color(slider, lv_color_hex(0x3C3C3C),
                                 LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(slider, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(slider, 3, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(slider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_left(slider, 5, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_right(slider, 5, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(slider, 5, LV_PART_KNOB | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_bottom(slider, 5, LV_PART_KNOB | LV_STATE_DEFAULT);

  return slider;
}

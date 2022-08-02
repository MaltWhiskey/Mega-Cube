// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: LEDCUBELCD

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_Panel1;
lv_obj_t * ui_Button1;
lv_obj_t * ui_Label1;
lv_obj_t * ui_Panel2;
lv_obj_t * ui_Button2;
lv_obj_t * ui_Label2;
lv_obj_t * ui_Panel3;
lv_obj_t * ui_Button3;
lv_obj_t * ui_Label3;
lv_obj_t * ui_Panel4;
lv_obj_t * ui_Button4;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Panel5;
lv_obj_t * ui_Button5;
lv_obj_t * ui_Label5;
lv_obj_t * ui_Panel6;
lv_obj_t * ui_Button6;
lv_obj_t * ui_Label6;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_Button7;
lv_obj_t * ui_Button8;
lv_obj_t * ui_Button9;
lv_obj_t * ui_Button10;
lv_obj_t * ui_Button11;
lv_obj_t * ui_Button12;
lv_obj_t * ui_Button13;
lv_obj_t * ui_Button14;
lv_obj_t * ui_Button15;
lv_obj_t * ui_Button16;
lv_obj_t * ui_Button17;
lv_obj_t * ui_Button18;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "#error LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
static void ui_event_Button1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        Event_Play_All(e);
    }
    if(event == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 1000, 0);
    }
}
static void ui_event_Button2(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        Event_Play_Accelerometer(e);
    }
    if(event == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 1000, 0);
    }
}
static void ui_event_Button3(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        Event_Play_Arrows(e);
    }
    if(event == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 1000, 0);
    }
}
static void ui_event_Button4(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        Event_Play_Atoms(e);
    }
    if(event == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 1000, 0);
    }
}
static void ui_event_Button5(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        Event_Play_Helix(e);
    }
    if(event == LV_EVENT_LONG_PRESSED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 1000, 0);
    }
}
static void ui_event_Button6(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 1000, 0);
    }
}
static void ui_event_Button18(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{

    // ui_Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel1

    ui_Panel1 = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_Panel1, 100);
    lv_obj_set_height(ui_Panel1, 114);

    lv_obj_set_x(ui_Panel1, 5);
    lv_obj_set_y(ui_Panel1, 4);

    lv_obj_clear_flag(ui_Panel1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Panel1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button1

    ui_Button1 = lv_btn_create(ui_Panel1);

    lv_obj_set_width(ui_Button1, 80);
    lv_obj_set_height(ui_Button1, 80);

    lv_obj_set_x(ui_Button1, 0);
    lv_obj_set_y(ui_Button1, 6);

    lv_obj_set_align(ui_Button1, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button1, ui_event_Button1, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_Button1, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button1, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button1, &ui_img_settings_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button1, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button1, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button1, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button1, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label1

    ui_Label1 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_Label1, 100);
    lv_obj_set_height(ui_Label1, 20);

    lv_obj_set_x(ui_Label1, 0);
    lv_obj_set_y(ui_Label1, 0);

    lv_obj_set_align(ui_Label1, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label1, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label1, "Play All");

    lv_obj_set_style_text_color(ui_Label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel2

    ui_Panel2 = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_Panel2, 100);
    lv_obj_set_height(ui_Panel2, 114);

    lv_obj_set_x(ui_Panel2, 110);
    lv_obj_set_y(ui_Panel2, 4);

    lv_obj_clear_flag(ui_Panel2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Panel2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button2

    ui_Button2 = lv_btn_create(ui_Panel2);

    lv_obj_set_width(ui_Button2, 80);
    lv_obj_set_height(ui_Button2, 80);

    lv_obj_set_x(ui_Button2, 0);
    lv_obj_set_y(ui_Button2, 6);

    lv_obj_set_align(ui_Button2, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button2, ui_event_Button2, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_Button2, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button2, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button2, &ui_img_accelerometer_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button2, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button2, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button2, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button2, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label2

    ui_Label2 = lv_label_create(ui_Panel2);

    lv_obj_set_width(ui_Label2, 100);
    lv_obj_set_height(ui_Label2, 20);

    lv_obj_set_x(ui_Label2, 0);
    lv_obj_set_y(ui_Label2, 0);

    lv_obj_set_align(ui_Label2, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label2, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label2, "Accelerometer");

    lv_obj_set_style_text_color(ui_Label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel3

    ui_Panel3 = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_Panel3, 100);
    lv_obj_set_height(ui_Panel3, 114);

    lv_obj_set_x(ui_Panel3, 215);
    lv_obj_set_y(ui_Panel3, 4);

    lv_obj_clear_flag(ui_Panel3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Panel3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button3

    ui_Button3 = lv_btn_create(ui_Panel3);

    lv_obj_set_width(ui_Button3, 80);
    lv_obj_set_height(ui_Button3, 80);

    lv_obj_set_x(ui_Button3, 0);
    lv_obj_set_y(ui_Button3, 6);

    lv_obj_set_align(ui_Button3, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_Button3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button3, ui_event_Button3, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_Button3, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button3, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button3, &ui_img_arrows_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button3, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button3, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button3, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button3, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label3

    ui_Label3 = lv_label_create(ui_Panel3);

    lv_obj_set_width(ui_Label3, 100);
    lv_obj_set_height(ui_Label3, 20);

    lv_obj_set_x(ui_Label3, 0);
    lv_obj_set_y(ui_Label3, 0);

    lv_obj_set_align(ui_Label3, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label3, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label3, "Arrows");

    lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel4

    ui_Panel4 = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_Panel4, 100);
    lv_obj_set_height(ui_Panel4, 114);

    lv_obj_set_x(ui_Panel4, 5);
    lv_obj_set_y(ui_Panel4, 122);

    lv_obj_clear_flag(ui_Panel4, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Panel4, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button4

    ui_Button4 = lv_btn_create(ui_Panel4);

    lv_obj_set_width(ui_Button4, 80);
    lv_obj_set_height(ui_Button4, 80);

    lv_obj_set_x(ui_Button4, 0);
    lv_obj_set_y(ui_Button4, 6);

    lv_obj_set_align(ui_Button4, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_Button4, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button4, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button4, ui_event_Button4, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_Button4, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button4, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button4, &ui_img_atoms_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button4, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button4, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button4, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button4, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label4

    ui_Label4 = lv_label_create(ui_Panel4);

    lv_obj_set_width(ui_Label4, 100);
    lv_obj_set_height(ui_Label4, 20);

    lv_obj_set_x(ui_Label4, 0);
    lv_obj_set_y(ui_Label4, 0);

    lv_obj_set_align(ui_Label4, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label4, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label4, "Athoms");

    lv_obj_set_style_text_color(ui_Label4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel5

    ui_Panel5 = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_Panel5, 100);
    lv_obj_set_height(ui_Panel5, 114);

    lv_obj_set_x(ui_Panel5, 110);
    lv_obj_set_y(ui_Panel5, 122);

    lv_obj_clear_flag(ui_Panel5, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Panel5, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button5

    ui_Button5 = lv_btn_create(ui_Panel5);

    lv_obj_set_width(ui_Button5, 80);
    lv_obj_set_height(ui_Button5, 80);

    lv_obj_set_x(ui_Button5, 0);
    lv_obj_set_y(ui_Button5, 6);

    lv_obj_set_align(ui_Button5, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_Button5, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button5, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button5, ui_event_Button5, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_Button5, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button5, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button5, &ui_img_fireworks_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button5, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button5, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button5, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button5, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button5, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label5

    ui_Label5 = lv_label_create(ui_Panel5);

    lv_obj_set_width(ui_Label5, 100);
    lv_obj_set_height(ui_Label5, 20);

    lv_obj_set_x(ui_Label5, 0);
    lv_obj_set_y(ui_Label5, 0);

    lv_obj_set_align(ui_Label5, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label5, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label5, "Fireworks");

    lv_obj_set_style_text_color(ui_Label5, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel6

    ui_Panel6 = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_Panel6, 100);
    lv_obj_set_height(ui_Panel6, 114);

    lv_obj_set_x(ui_Panel6, 215);
    lv_obj_set_y(ui_Panel6, 122);

    lv_obj_clear_flag(ui_Panel6, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Panel6, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button6

    ui_Button6 = lv_btn_create(ui_Panel6);

    lv_obj_set_width(ui_Button6, 80);
    lv_obj_set_height(ui_Button6, 80);

    lv_obj_set_x(ui_Button6, 0);
    lv_obj_set_y(ui_Button6, 6);

    lv_obj_set_align(ui_Button6, LV_ALIGN_TOP_MID);

    lv_obj_add_flag(ui_Button6, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button6, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button6, ui_event_Button6, LV_EVENT_ALL, NULL);
    lv_obj_set_style_radius(ui_Button6, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Button6, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Button6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button6, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button6, &ui_img_next_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button6, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button6, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button6, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button6, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button6, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label6

    ui_Label6 = lv_label_create(ui_Panel6);

    lv_obj_set_width(ui_Label6, 100);
    lv_obj_set_height(ui_Label6, 20);

    lv_obj_set_x(ui_Label6, 0);
    lv_obj_set_y(ui_Label6, 0);

    lv_obj_set_align(ui_Label6, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label6, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label6, "Next page");

    lv_obj_set_style_text_color(ui_Label6, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_Screen2_screen_init(void)
{

    // ui_Screen2

    ui_Screen2 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Button7

    ui_Button7 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button7, 64);
    lv_obj_set_height(ui_Button7, 64);

    lv_obj_set_x(ui_Button7, 0);
    lv_obj_set_y(ui_Button7, 0);

    lv_obj_add_flag(ui_Button7, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button7, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button7, &ui_img_helix_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button8

    ui_Button8 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button8, 64);
    lv_obj_set_height(ui_Button8, 64);

    lv_obj_set_x(ui_Button8, 64);
    lv_obj_set_y(ui_Button8, 0);

    lv_obj_add_flag(ui_Button8, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button8, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button8, &ui_img_life_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button9

    ui_Button9 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button9, 64);
    lv_obj_set_height(ui_Button9, 64);

    lv_obj_set_x(ui_Button9, 128);
    lv_obj_set_y(ui_Button9, 0);

    lv_obj_add_flag(ui_Button9, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button9, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button9, &ui_img_mario_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button10

    ui_Button10 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button10, 64);
    lv_obj_set_height(ui_Button10, 64);

    lv_obj_set_x(ui_Button10, 192);
    lv_obj_set_y(ui_Button10, 0);

    lv_obj_add_flag(ui_Button10, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button10, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button10, &ui_img_plasma_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button11

    ui_Button11 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button11, 64);
    lv_obj_set_height(ui_Button11, 64);

    lv_obj_set_x(ui_Button11, 256);
    lv_obj_set_y(ui_Button11, 0);

    lv_obj_add_flag(ui_Button11, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button11, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button11, &ui_img_pong_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button12

    ui_Button12 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button12, 64);
    lv_obj_set_height(ui_Button12, 64);

    lv_obj_set_x(ui_Button12, 0);
    lv_obj_set_y(ui_Button12, 64);

    lv_obj_add_flag(ui_Button12, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button12, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button12, &ui_img_sinus_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button13

    ui_Button13 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button13, 64);
    lv_obj_set_height(ui_Button13, 64);

    lv_obj_set_x(ui_Button13, 64);
    lv_obj_set_y(ui_Button13, 64);

    lv_obj_add_flag(ui_Button13, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button13, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button13, &ui_img_spectrum_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button14

    ui_Button14 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button14, 64);
    lv_obj_set_height(ui_Button14, 64);

    lv_obj_set_x(ui_Button14, 128);
    lv_obj_set_y(ui_Button14, 64);

    lv_obj_add_flag(ui_Button14, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button14, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button14, &ui_img_starfield_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button15

    ui_Button15 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button15, 64);
    lv_obj_set_height(ui_Button15, 64);

    lv_obj_set_x(ui_Button15, 192);
    lv_obj_set_y(ui_Button15, 64);

    lv_obj_add_flag(ui_Button15, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button15, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button15, &ui_img_twinkels1_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button16

    ui_Button16 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button16, 64);
    lv_obj_set_height(ui_Button16, 64);

    lv_obj_set_x(ui_Button16, 256);
    lv_obj_set_y(ui_Button16, 64);

    lv_obj_add_flag(ui_Button16, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button16, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button16, &ui_img_twinkels2_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button17

    ui_Button17 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button17, 64);
    lv_obj_set_height(ui_Button17, 64);

    lv_obj_set_x(ui_Button17, 239);
    lv_obj_set_y(ui_Button17, 137);

    lv_obj_add_flag(ui_Button17, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button17, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(ui_Button17, &ui_img_scroller_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Button18

    ui_Button18 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button18, 212);
    lv_obj_set_height(ui_Button18, 120);

    lv_obj_set_x(ui_Button18, 0);
    lv_obj_set_y(ui_Button18, 0);

    lv_obj_set_align(ui_Button18, LV_ALIGN_BOTTOM_LEFT);

    lv_obj_add_flag(ui_Button18, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button18, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button18, ui_event_Button18, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_img_src(ui_Button18, &ui_img_maltwhiskey_png, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_basic_init(dispp);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    lv_disp_load_scr(ui_Screen1);
}


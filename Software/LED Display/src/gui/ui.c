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

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "#error LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{

    // ui_Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

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

    lv_obj_add_state(ui_Button1, LV_STATE_PRESSED);

    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_radius(ui_Button1, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_Button1, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Button1, &ui_img_accelerometer_80_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui_Button1, 100, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_color(ui_Button1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_opa(ui_Button1, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui_Button1, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_spread(ui_Button1, 3, LV_PART_MAIN | LV_STATE_PRESSED);

    // ui_Label1

    ui_Label1 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_Label1, lv_pct(100));
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label1, 0);
    lv_obj_set_y(ui_Label1, 0);

    lv_obj_set_align(ui_Label1, LV_ALIGN_BOTTOM_MID);

    lv_label_set_long_mode(ui_Label1, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ui_Label1, "Accelerometer");

    lv_obj_set_style_text_color(ui_Label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_basic_init(dispp);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);
}


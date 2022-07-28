// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_Button1;
lv_obj_t * ui_Label1;
lv_obj_t * ui_Button2;
lv_obj_t * ui_Label2;
lv_obj_t * ui_Button3;
lv_obj_t * ui_Label3;
lv_obj_t * ui_Button4;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Button5;
lv_obj_t * ui_Label5;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_Button6;
lv_obj_t * ui_Label6;
lv_obj_t * ui_Button7;
lv_obj_t * ui_Label7;
lv_obj_t * ui_Button8;
lv_obj_t * ui_Label8;
lv_obj_t * ui_Button9;
lv_obj_t * ui_Label9;
lv_obj_t * ui_Button10;
lv_obj_t * ui_Label10;
lv_obj_t * ui_Screen3;
lv_obj_t * ui_Button14;
lv_obj_t * ui_Label14;
lv_obj_t * ui_Slider1;

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
        EVENT_ARROWS(e);
        (e);
    }
}
static void ui_event_Button2(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        EVENT_FIREWORKS(e);
    }
}
static void ui_event_Button3(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        EVENT_ATOMS(e);
    }
}
static void ui_event_Button4(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
static void ui_event_Button6(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        EVENT_ARROWS(e);
        (e);
    }
}
static void ui_event_Button7(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        EVENT_FIREWORKS(e);
    }
}
static void ui_event_Button8(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        EVENT_ATOMS(e);
    }
}
static void ui_event_Button9(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
static void ui_event_Button14(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
static void ui_event_Slider1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_VALUE_CHANGED) {
        EVENT_SLIDER_CHG(e);
    }
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{

    // ui_Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Button1

    ui_Button1 = lv_btn_create(ui_Screen1);

    lv_obj_set_width(ui_Button1, 80);
    lv_obj_set_height(ui_Button1, 100);

    lv_obj_set_x(ui_Button1, 20);
    lv_obj_set_y(ui_Button1, 20);

    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button1, ui_event_Button1, LV_EVENT_ALL, NULL);

    // ui_Label1

    ui_Label1 = lv_label_create(ui_Button1);

    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label1, 0);
    lv_obj_set_y(ui_Label1, 0);

    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label1, "Arrows");

    // ui_Button2

    ui_Button2 = lv_btn_create(ui_Screen1);

    lv_obj_set_width(ui_Button2, 80);
    lv_obj_set_height(ui_Button2, 100);

    lv_obj_set_x(ui_Button2, 120);
    lv_obj_set_y(ui_Button2, 20);

    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button2, ui_event_Button2, LV_EVENT_ALL, NULL);

    // ui_Label2

    ui_Label2 = lv_label_create(ui_Button2);

    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label2, 0);
    lv_obj_set_y(ui_Label2, 0);

    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label2, "Fireworks");

    // ui_Button3

    ui_Button3 = lv_btn_create(ui_Screen1);

    lv_obj_set_width(ui_Button3, 80);
    lv_obj_set_height(ui_Button3, 100);

    lv_obj_set_x(ui_Button3, 220);
    lv_obj_set_y(ui_Button3, 20);

    lv_obj_add_flag(ui_Button3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button3, ui_event_Button3, LV_EVENT_ALL, NULL);

    // ui_Label3

    ui_Label3 = lv_label_create(ui_Button3);

    lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label3, 0);
    lv_obj_set_y(ui_Label3, 0);

    lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label3, "Atoms");

    // ui_Button4

    ui_Button4 = lv_btn_create(ui_Screen1);

    lv_obj_set_width(ui_Button4, 120);
    lv_obj_set_height(ui_Button4, 60);

    lv_obj_set_x(ui_Button4, 180);
    lv_obj_set_y(ui_Button4, 160);

    lv_obj_add_flag(ui_Button4, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button4, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button4, ui_event_Button4, LV_EVENT_ALL, NULL);

    // ui_Label4

    ui_Label4 = lv_label_create(ui_Button4);

    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label4, 0);
    lv_obj_set_y(ui_Label4, 0);

    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label4, ">>");

    // ui_Button5

    ui_Button5 = lv_btn_create(ui_Screen1);

    lv_obj_set_width(ui_Button5, 120);
    lv_obj_set_height(ui_Button5, 60);

    lv_obj_set_x(ui_Button5, 20);
    lv_obj_set_y(ui_Button5, 160);

    lv_obj_add_flag(ui_Button5, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button5, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_grad_color(ui_Button5, lv_color_hex(0x1C00FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Button5, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Label5

    ui_Label5 = lv_label_create(ui_Button5);

    lv_obj_set_width(ui_Label5, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label5, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label5, 0);
    lv_obj_set_y(ui_Label5, 0);

    lv_obj_set_align(ui_Label5, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label5, "Settings");

}
void ui_Screen2_screen_init(void)
{

    // ui_Screen2

    ui_Screen2 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Button6

    ui_Button6 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button6, 80);
    lv_obj_set_height(ui_Button6, 100);

    lv_obj_set_x(ui_Button6, 20);
    lv_obj_set_y(ui_Button6, 20);

    lv_obj_add_flag(ui_Button6, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button6, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button6, ui_event_Button6, LV_EVENT_ALL, NULL);

    // ui_Label6

    ui_Label6 = lv_label_create(ui_Button6);

    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label6, 0);
    lv_obj_set_y(ui_Label6, 0);

    lv_obj_set_align(ui_Label6, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label6, "Helix");

    // ui_Button7

    ui_Button7 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button7, 80);
    lv_obj_set_height(ui_Button7, 100);

    lv_obj_set_x(ui_Button7, 120);
    lv_obj_set_y(ui_Button7, 20);

    lv_obj_add_flag(ui_Button7, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button7, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button7, ui_event_Button7, LV_EVENT_ALL, NULL);

    // ui_Label7

    ui_Label7 = lv_label_create(ui_Button7);

    lv_obj_set_width(ui_Label7, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label7, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label7, 0);
    lv_obj_set_y(ui_Label7, 0);

    lv_obj_set_align(ui_Label7, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label7, "Life");

    // ui_Button8

    ui_Button8 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button8, 80);
    lv_obj_set_height(ui_Button8, 100);

    lv_obj_set_x(ui_Button8, 220);
    lv_obj_set_y(ui_Button8, 20);

    lv_obj_add_flag(ui_Button8, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button8, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button8, ui_event_Button8, LV_EVENT_ALL, NULL);

    // ui_Label8

    ui_Label8 = lv_label_create(ui_Button8);

    lv_obj_set_width(ui_Label8, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label8, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label8, 0);
    lv_obj_set_y(ui_Label8, 0);

    lv_obj_set_align(ui_Label8, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label8, "Mario");

    // ui_Button9

    ui_Button9 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button9, 120);
    lv_obj_set_height(ui_Button9, 60);

    lv_obj_set_x(ui_Button9, 180);
    lv_obj_set_y(ui_Button9, 160);

    lv_obj_add_flag(ui_Button9, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button9, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button9, ui_event_Button9, LV_EVENT_ALL, NULL);

    // ui_Label9

    ui_Label9 = lv_label_create(ui_Button9);

    lv_obj_set_width(ui_Label9, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label9, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label9, 0);
    lv_obj_set_y(ui_Label9, 0);

    lv_obj_set_align(ui_Label9, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label9, ">>");

    // ui_Button10

    ui_Button10 = lv_btn_create(ui_Screen2);

    lv_obj_set_width(ui_Button10, 120);
    lv_obj_set_height(ui_Button10, 60);

    lv_obj_set_x(ui_Button10, 20);
    lv_obj_set_y(ui_Button10, 160);

    lv_obj_add_flag(ui_Button10, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button10, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_grad_color(ui_Button10, lv_color_hex(0x1C00FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Button10, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Label10

    ui_Label10 = lv_label_create(ui_Button10);

    lv_obj_set_width(ui_Label10, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label10, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label10, 0);
    lv_obj_set_y(ui_Label10, 0);

    lv_obj_set_align(ui_Label10, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label10, "Settings");

}
void ui_Screen3_screen_init(void)
{

    // ui_Screen3

    ui_Screen3 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Button14

    ui_Button14 = lv_btn_create(ui_Screen3);

    lv_obj_set_width(ui_Button14, 120);
    lv_obj_set_height(ui_Button14, 60);

    lv_obj_set_x(ui_Button14, 180);
    lv_obj_set_y(ui_Button14, 160);

    lv_obj_add_flag(ui_Button14, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_Button14, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_Button14, ui_event_Button14, LV_EVENT_ALL, NULL);

    // ui_Label14

    ui_Label14 = lv_label_create(ui_Button14);

    lv_obj_set_width(ui_Label14, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label14, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label14, 0);
    lv_obj_set_y(ui_Label14, 0);

    lv_obj_set_align(ui_Label14, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label14, ">>");

    // ui_Slider1

    ui_Slider1 = lv_slider_create(ui_Screen3);
    lv_slider_set_range(ui_Slider1, 0, 100);

    lv_obj_set_width(ui_Slider1, 150);
    lv_obj_set_height(ui_Slider1, 10);

    lv_obj_set_x(ui_Slider1, 0);
    lv_obj_set_y(ui_Slider1, 0);

    lv_obj_set_align(ui_Slider1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Slider1, ui_event_Slider1, LV_EVENT_ALL, NULL);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    lv_disp_load_scr(ui_Screen1);
}


/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "glob.h"

static lv_display_t * hal_init(int32_t w, int32_t h);



/******************
Speedometer
******************/

LV_IMAGE_DECLARE(main_needle);
lv_obj_t * needle_img;

static void set_needle_img_value(void * obj, int32_t v)
{
    /* Speedometer needle Value */
    lv_scale_set_image_needle_value(obj, needle_img, v);
}

void speedo_needle_img(lv_obj_t * obj)
{
    /* Speedometer Needle*/
    needle_img = lv_image_create(obj);
    lv_image_set_src(needle_img, &main_needle);
    lv_image_set_pivot(needle_img, 80,30);// adjust this
    lv_obj_align(needle_img, LV_ALIGN_CENTER, 55, 0);// adjust this


}

static void arc_image(void)
{
    /** Speedometer effects and BG */
    lv_obj_t * arc = lv_arc_create(lv_screen_active());

    LV_IMAGE_DECLARE(Main_speedometer); // Replace with your image
    static lv_style_t style_main;
    lv_style_init(&style_main);
    lv_style_set_arc_image_src(&style_main, &Main_speedometer); // Set image as source
    lv_style_set_bg_image_src(&style_main, &Main_speedometer); // Set image as source
    lv_obj_remove_style(arc, NULL, LV_PART_MAIN| LV_PART_KNOB);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(arc, 550, 550);
    lv_obj_add_style(arc, &style_main, LV_PART_MAIN);
    lv_arc_set_value(arc,0);


}


// function to create the speedometer
static void main_speedometer(void)
{

    arc_image();

    /* Speedometer Main body */
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    lv_obj_remove_style_all(scale);
    lv_obj_set_size(scale, 550, 550);
    lv_scale_set_label_show(scale, false);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(scale, 0, 100);
    lv_scale_set_angle_range(scale, 180+45);
    lv_obj_align(scale, LV_ALIGN_CENTER, 0, 0);
    speedo_needle_img(scale);

}


/******************
Toggle Button
******************/

static void button_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e); // Get the button object
    lv_obj_t * label = lv_obj_get_child(btn, 0); // Get the first child of the button, which is the label

    if(code == LV_EVENT_CLICKED) {
        const char * current_text = lv_label_get_text(label);

        if(strcmp(current_text, "Start") == 0) {
            lv_label_set_text_fmt(label, "Stop"); // Change to "Stop" if it is "Start"
        } else {
            lv_label_set_text_fmt(label, "Start"); // Change back to "Start" otherwise
        }

        LV_LOG_USER("Button clicked. Current state: %s", lv_label_get_text(label));
    }
}

/******************
 * Motor Temperature Bar
******************/

void func_motor_temp_bar(void)
{
    static lv_style_t motor_temp_style_indic;

    lv_style_init(&motor_temp_style_indic);
    lv_style_set_bg_opa(&motor_temp_style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&motor_temp_style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&motor_temp_style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&motor_temp_style_indic, LV_GRAD_DIR_VER);

    lv_obj_t * motor_temp_bar = lv_bar_create(lv_screen_active());
    lv_obj_add_style(motor_temp_bar, &motor_temp_style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(motor_temp_bar, 40, 400);
    lv_obj_align(motor_temp_bar, LV_ALIGN_LEFT_MID, 80, 0);
    lv_bar_set_range(motor_temp_bar, -20, 40);

}

/******************
 * Regen brake Bar
******************/

void func_regen_brake_bar(void)
{
    static lv_style_t regen_brake_style_indic;

    lv_style_init(&regen_brake_style_indic);
    lv_style_set_bg_opa(&regen_brake_style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&regen_brake_style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&regen_brake_style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&regen_brake_style_indic, LV_GRAD_DIR_VER);

    lv_obj_t * regen_brake = lv_bar_create(lv_screen_active());
    lv_obj_add_style(regen_brake, &regen_brake_style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(regen_brake, 40, 400);
    lv_obj_align(regen_brake, LV_ALIGN_RIGHT_MID, -80, 0);
    lv_bar_set_range(regen_brake, -20, 40);

}


/******************
Start/Stop Button
******************/

void create_start_stop_button(void)
{
    lv_obj_t *button_label;

    lv_obj_t *start_stop_button = lv_btn_create(lv_screen_active());
    lv_obj_add_event_cb(start_stop_button, button_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(start_stop_button, LV_ALIGN_TOP_MID, 350, 80);
    lv_obj_add_flag(start_stop_button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(start_stop_button, LV_SIZE_CONTENT);

    button_label = lv_label_create(start_stop_button);
    lv_label_set_text(button_label, "Start"); // Initially set the label to "Start"
    lv_obj_center(button_label);

    /* Initialize the pressed state style */
    static lv_style_t pressed_style;
    lv_style_init(&pressed_style);

    /* Add a large outline when pressed */
    lv_style_set_outline_width(&pressed_style, 30);
    lv_style_set_outline_opa(&pressed_style, LV_OPA_TRANSP);

    lv_style_set_translate_y(&pressed_style, 5);
    lv_style_set_shadow_offset_y(&pressed_style, 3);
    lv_style_set_bg_color(&pressed_style, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&pressed_style, lv_palette_darken(LV_PALETTE_BLUE, 4));

    /* Add a transition to the outline */
    static lv_style_transition_dsc_t outline_transition;
    static lv_style_prop_t transition_properties[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&outline_transition, transition_properties, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&pressed_style, &outline_transition);
    lv_obj_add_style(start_stop_button, &pressed_style, LV_STATE_PRESSED);
    lv_obj_set_size(start_stop_button, 200, 100);
}

/******************
Status LED
******************/

static void lv_led_set_bright(lv_obj_t * led, uint8_t bright)
{
    lv_led_set_bright(led, bright);
}

void func_status_led(void)
{
    lv_obj_t * status_led_label;
    lv_obj_t * status_led = lv_led_create(lv_screen_active());
    lv_obj_remove_style_all(status_led);

    static lv_style_t style_warning;
    lv_style_init(&style_warning);
    lv_style_set_bg_color(&style_warning, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_border_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 3));
    lv_style_set_border_width(&style_warning, 2);
    lv_style_set_text_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 4));
    lv_style_set_radius(&style_warning, 10);
    lv_obj_add_style(status_led, &style_warning, 0);
    lv_obj_align(status_led, LV_ALIGN_TOP_MID, -350, 80);
    lv_obj_set_size(status_led, 200, 100);
    lv_led_on(status_led);

    status_led_label = lv_label_create(status_led);
    lv_label_set_text(status_led_label, "Warning");
    lv_obj_center(status_led_label);

}


/******************
 * Menu Button/Window
******************/

static void event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    LV_UNUSED(label);
    LV_LOG_USER("Button %s clicked", lv_label_get_text(label));
    // if it is close button, return the orginal window
    if(strcmp(lv_label_get_text(label), LV_SYMBOL_CLOSE) == 0) {
        lv_obj_t * win = lv_obj_get_parent(lv_obj_get_parent(btn));
        lv_obj_del(win);
    }
}

static void menu_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
        // // Create a new window
        // lv_obj_t * mbox1 = lv_msgbox_create(NULL);

        // lv_msgbox_add_title(mbox1, "Menu");

        // lv_msgbox_add_close_button(mbox1);

        // lv_obj_t * btn;
        // btn = lv_msgbox_add_footer_button(mbox1, "Settings");
        // lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
        // btn = lv_msgbox_add_footer_button(mbox1, "About");
        // lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
        // return;

    lv_obj_t * win = lv_win_create(lv_screen_active());
    lv_obj_t * btn;

    lv_win_add_title(win, "Menu");

    btn = lv_win_add_button(win, LV_SYMBOL_CLOSE, 60);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/

    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void func_menu_button(void)
{
    lv_obj_t * menu_label;

    lv_obj_t * menu_button = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(menu_button, menu_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(menu_button, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_remove_flag(menu_button, LV_OBJ_FLAG_PRESS_LOCK);

    menu_label = lv_label_create(menu_button);
    lv_label_set_text(menu_label, "Menu");
    lv_obj_center(menu_label);

}


extern void freertos_main(void);

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init(1280, 800);

  #if LV_USE_OS == LV_OS_NONE

  main_speedometer();
  create_start_stop_button();
  func_status_led();
  func_motor_temp_bar();
  func_regen_brake_bar();
  func_menu_button();
  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(5 * 1000);
  }

  #elif LV_USE_OS == LV_OS_FREERTOS

  /* Run FreeRTOS and create lvgl task */
  freertos_main();

  #endif

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t * hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t * kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}

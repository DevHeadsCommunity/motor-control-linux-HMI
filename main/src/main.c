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



    // /* speedometer needle animation */
    // lv_anim_t anim_scale_img;
    // lv_anim_init(&anim_scale_img);
    // lv_anim_set_var(&anim_scale_img, scale);
    // lv_anim_set_exec_cb(&anim_scale_img, set_needle_img_value);
    // lv_anim_set_path_cb(&anim_scale_img, lv_anim_path_ease_in_out);
    // lv_anim_set_playback_time(&anim_scale_img, 3000);
    // lv_anim_set_playback_delay(&anim_scale_img, 1000);

    // lv_anim_set_path_cb(&anim_scale_img, lv_anim_path_ease_in_out);

    // lv_anim_set_duration(&anim_scale_img, 3000);
    // lv_anim_set_repeat_count(&anim_scale_img, LV_ANIM_REPEAT_INFINITE);
    // lv_anim_set_values(&anim_scale_img, 00, 100);
    // lv_anim_start(&anim_scale_img);
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

void toggle_button(void)
{
    lv_obj_t * label;

    lv_obj_t * btn2 = lv_btn_create(lv_screen_active());
    lv_obj_add_event_cb(btn2, button_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_TOP_MID, 350, 80);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Start"); // Initially set the label to "Start"
    lv_obj_center(label);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_offset_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);
    lv_obj_add_style(btn2, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn2, 200, 100);

}

/******************
Status LED
******************/

static void lv_led_set_bright(lv_obj_t * led, uint8_t bright)
{
    lv_led_set_bright(led, bright);
}

void status_led(void)
{
    lv_obj_t * label;
    lv_obj_t * led1 = lv_led_create(lv_screen_active());
    lv_obj_remove_style_all(led1);

    static lv_style_t style_warning;
    lv_style_init(&style_warning);
    lv_style_set_bg_color(&style_warning, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_border_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 3));
    lv_style_set_border_width(&style_warning, 2);
    lv_style_set_text_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 4));
    lv_style_set_radius(&style_warning, 10);
    lv_obj_add_style(led1, &style_warning, 0);
    lv_obj_align(led1, LV_ALIGN_TOP_MID, -350, 80);
    lv_obj_set_size(led1, 200, 100);
    lv_led_on(led1);

    label = lv_label_create(led1);
    lv_label_set_text(label, "Warning");
    lv_obj_center(label);

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
  toggle_button();
  status_led();
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

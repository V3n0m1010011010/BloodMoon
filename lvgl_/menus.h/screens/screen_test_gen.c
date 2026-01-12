/**
 * @file screen_test_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "screen_test_gen.h"
#include "menus_h.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * screen_test_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_main;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_main);
        lv_style_set_bg_color(&style_main, lv_color_hex(0x8f4f4f));

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_name_static(lv_obj_0, "screen_test_#");

    lv_obj_add_style(lv_obj_0, &style_main, 0);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_0);
    lv_obj_set_align(lv_button_0, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(lv_button_0, lv_color_hex(0xf8f8f8), 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_0, "Hello world");
    lv_obj_set_style_text_font(lv_label_0, font_medium, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/*
 * Pebble Clicker - Tally counter for Pebble
 * Copyright (C) 2013 Grahame Todd
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x93, 0xED, 0xC7, 0xF5, 0xB2, 0x99, 0x41, 0x55, 0xB4, 0x26, 0x80, 0xF1, 0xC5, 0xAC, 0x7A, 0xF1 }
PBL_APP_INFO(MY_UUID,
             "Clicker", "Grahame Todd",
             3, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

static Window window;
AppContextRef app;

static TextLayer count_layer;
static BmpContainer button_labels;


int currentCount = 0;

#define FONT_BIG RESOURCE_ID_FONT_ROBOTO_BLACK_48

void config_provider(ClickConfig **config, Window *window);
void handle_init(AppContextRef ctx);

void pbl_main(void *params);

void count_up_handler(ClickRecognizerRef recognizer, Window *window);
void count_down_handler(ClickRecognizerRef recognizer, Window *window);
void select_handler(ClickRecognizerRef recognizer, Window *window);

void update_count();
void itoa2(int num, char* buffer);

void handle_init(AppContextRef ctx) {
    app = ctx;

    window_init(&window, "Clicker");
    window_stack_push(&window, true);
    window_set_background_color(&window, GColorBlack);
    window_set_fullscreen(&window, false);

    resource_init_current_app(&APP_RESOURCES);

    window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);

    GFont big_font = fonts_load_custom_font(resource_get_handle(FONT_BIG));
    
    Layer *root_layer = window_get_root_layer(&window);

    // Count layer
    text_layer_init(&count_layer, GRect(0, 40, 100, 90));
    text_layer_set_background_color(&count_layer, GColorBlack);
    text_layer_set_font(&count_layer, big_font);
    text_layer_set_text_color(&count_layer, GColorWhite);
    text_layer_set_text(&count_layer, "00");
    text_layer_set_text_alignment(&count_layer, GTextAlignmentRight);
    layer_add_child(root_layer, &count_layer.layer);


    // Button labels
    bmp_init_container(RESOURCE_ID_IMAGE_BUTTON_LABELS, &button_labels);
    layer_set_frame(&button_labels.layer.layer, GRect(127, 10, 17, 136));
    layer_add_child(root_layer, &button_labels.layer.layer);

}

void handle_deinit(AppContextRef ctx) {
    bmp_deinit_container(&button_labels);
}

void draw_line(Layer *me, GContext* ctx) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_line(ctx, GPoint(0, 0), GPoint(140, 0));
    graphics_draw_line(ctx, GPoint(0, 1), GPoint(140, 1));
}

void changeCount(int d) {
    if(d == 0)
    {
        currentCount = 0;
    }
    else
    {
        currentCount = currentCount + d;
        if(currentCount < 0)
        {
            currentCount = 0;
        }
        if(currentCount > 99)
        {
            currentCount = 99;
        }
    }
    
    update_count();
}

void count_up_handler(ClickRecognizerRef recognizer, Window *window) {
    changeCount(1);
}

void count_down_handler(ClickRecognizerRef recognizer, Window *window) {
    changeCount(-1);
}

void select_handler(ClickRecognizerRef recognizer, Window *window) {
    changeCount(0);
}

void update_count()
{
	static char counter[] = "00";

	itoa2(currentCount, &counter[0]);
    
    text_layer_set_text(&count_layer, counter);
}


void config_provider(ClickConfig **config, Window *window) {
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler)select_handler;
    config[BUTTON_ID_UP]->click.handler = (ClickHandler)count_up_handler;
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler)count_down_handler;
    
	(void)window;
}

void itoa2(int num, char* buffer) {
    const char digits[10] = "0123456789";
    if(num > 9) {
        buffer[0] = digits[num / 10];
    } else {
        buffer[0] = '0';
    }
    buffer[1] = digits[num % 10];
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}

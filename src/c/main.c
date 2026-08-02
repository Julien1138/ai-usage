#include <pebble.h>

// AI Usage — jauges d'utilisation Claude.ai sur Pebble.
// main.c : cycle de vie de l'app, fenêtre, boutons, timer.

#include "comm.h"
#include "gauges.h"
#include "i18n.h"
#include "theme.h"
#include "ui.h"

static Window *s_window;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (gauges_count() == 0) {
    gauges_set_status(i18n_get(STR_REFRESHING));
    ui_mark_dirty();
  }
  comm_request_refresh();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  ui_scroll(+1);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  ui_scroll(-1);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_click_handler);
}

static void window_load(Window *window) {
  ui_window_load(window);
}

static void window_unload(Window *window) {
  ui_window_unload(window);
}

static void init(void) {
  gauges_load_persisted();

  s_window = window_create();
  window_set_background_color(s_window, COLOR_BG);
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  comm_init();
  ui_start_clock();
}

static void deinit(void) {
  ui_stop_clock();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

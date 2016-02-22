#include "main_window.h"

#define MAX_METRICS HealthMetricActiveKCalories

static Window *s_window;
static TextLayer *s_value_layer, *s_label_layer;

static HealthMetric s_metric;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) { 
  s_metric -= (s_metric > 0) ? 1 : -(MAX_METRICS);
  main_window_update_ui();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) { 
  s_metric += (s_metric < MAX_METRICS) ? 1 : -(MAX_METRICS);
  main_window_update_ui();
}
  
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static TextLayer* make_text_layer(int y_inset, char *font_key) {
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);

  TextLayer *this = text_layer_create(grect_inset(bounds, 
                                                GEdgeInsets(y_inset, 0, 0, 0)));
  text_layer_set_text_alignment(this, GTextAlignmentCenter);
  text_layer_set_text_color(this, GColorWhite);
  text_layer_set_background_color(this, GColorClear);
  text_layer_set_font(this, fonts_get_system_font(font_key));

#if defined(PBL_ROUND)
  text_layer_enable_screen_text_flow_and_paging(this, 5);
#endif

  return this;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_value_layer = make_text_layer(50, FONT_KEY_GOTHIC_28_BOLD);
  s_label_layer = make_text_layer(80, FONT_KEY_GOTHIC_24_BOLD);
  layer_add_child(window_layer, text_layer_get_layer(s_value_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_value_layer);
  text_layer_destroy(s_label_layer);

  window_destroy(s_window);
  s_window = NULL;
}

void main_window_push() {
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }
  window_stack_push(s_window, true);

  main_window_update_ui();
}

static void set_ui_values(char *label_text, GColor bg_color) {
  text_layer_set_text(s_label_layer, label_text);
  window_set_background_color(s_window, bg_color);
}

void main_window_update_ui() {
  if(health_is_available() && s_window) {
    static char s_value_buffer[8];

    snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", 
                                              health_get_metric_sum(s_metric));

    switch(s_metric) {
      case HealthMetricStepCount:
        set_ui_values("Steps taken today", GColorWindsorTan);
        break;
      case HealthMetricActiveSeconds:
        set_ui_values("Seconds active today", GColorDarkGreen);
        break;
      case HealthMetricWalkedDistanceMeters:
        set_ui_values("Meters travelled today", GColorJazzberryJam);
        break;
      case HealthMetricSleepSeconds:
        set_ui_values("Seconds asleep today", GColorBlueMoon);
        break;
      case HealthMetricSleepRestfulSeconds:
        set_ui_values("Restful sleep today", GColorDukeBlue);
        break;
      case HealthMetricRestingKCalories:
        set_ui_values("Resting kcal today", GColorCadetBlue);
        break;
      case HealthMetricActiveKCalories:
        set_ui_values("Active kcal today", GColorMidnightGreen);
        break;
      default: 
        break;
    }

    text_layer_set_text(s_value_layer, s_value_buffer);
  } else {
    set_ui_values("Health not available!", GColorDarkCandyAppleRed);
  }
}

#include <pebble.h>

#include "modules/health.h"

#include "windows/main_window.h"

static void init() {
  health_init();
  main_window_push();
}

static void deinit() { }

int main() {
  init();
  app_event_loop();
  deinit();
}

#include "health.h"

static bool s_health_available;

static void health_handler(HealthEventType event, void *context) {
  main_window_update_ui();
}

void health_init() {
  s_health_available = health_service_events_subscribe(health_handler, NULL);
  if(!s_health_available) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
}

bool health_is_available() {
  return s_health_available;
}

int health_get_metric_sum(HealthMetric metric) {
  return (int)health_service_sum_today(metric);
}

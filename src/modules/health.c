#include "health.h"

static bool s_health_available;

static void health_handler(HealthEventType event, void *context) {
  main_window_update_ui();
}

static void test() {
  const HealthMetric metric = HealthMetricWalkedDistanceMeters;
  const HealthValue distance = health_service_sum_today(metric);

  // Get the preferred measurement system
  MeasurementSystem system = health_service_get_measurement_system_for_display(
                                                                          metric);

  // Format accordingly
  static char s_buffer[32];
  switch(system) {
    case MeasurementSystemMetric:
      APP_LOG(APP_LOG_LEVEL_INFO, "Walked %d meters", (int)distance);    
      break;
    case MeasurementSystemImperial: {
      // Convert to imperial first
      int feet = (int)((float)distance * 3.28F);
      APP_LOG(APP_LOG_LEVEL_INFO, "Walked %d feet", (int)feet);
    } break;
    case MeasurementSystemUnknown:
    default:
      APP_LOG(APP_LOG_LEVEL_INFO, "MeasurementSystem unknown or does not apply");
  }

  // Display to user in correct units
  // text_layer_set_text(s_some_layer, s_buffer);
}

void health_init() {
  s_health_available = health_service_events_subscribe(health_handler, NULL);
  if(!s_health_available) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }

  test();
}

bool health_is_available() {
  return s_health_available;
}

int health_get_metric_sum(HealthMetric metric) {
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
    time_start_of_today(), time(NULL));
  if(mask == HealthServiceAccessibilityMaskAvailable) {
    return (int)health_service_sum_today(metric);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
    return 0;
  }
}

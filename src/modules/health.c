#include "health.h"

static HealthValue s_steps, s_active_time, s_distance, s_sleep;
static time_t s_launch_time;
static bool s_health_available;

static HealthValue get_metric_if_available(HealthMetric metric) {
  time_t time_now = time(NULL);
  
  HealthServiceAccessibilityMask result = 
      health_service_metric_accessible(metric, s_launch_time, time_now);
  if(result != HealthServiceAccessibilityMaskAvailable) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "No data available for metric %d: reason: %d", 
            (int)metric, (int)result);
  }

  return health_service_sum(metric, s_launch_time, time_now);
}

static void read_activity() {
  s_steps = get_metric_if_available(HealthMetricStepCount);
  s_active_time = get_metric_if_available(HealthMetricActiveSeconds);
  s_distance = get_metric_if_available(HealthMetricWalkedDistanceMeters);
}

static void read_sleep() {
  s_sleep = get_metric_if_available(HealthMetricSleepSeconds);
}

static void health_handler(HealthEventType event, void *context) {
  switch(event) {
    case HealthEventSignificantUpdate:
      read_activity();
      read_sleep();
      break;
    case HealthEventMovementUpdate:
      read_activity();
      break;
    case HealthEventSleepUpdate:
      read_sleep();
      break;
  }

  main_window_update_ui();
}

void health_init() {
  s_launch_time = time(NULL);

  s_health_available = health_service_events_subscribe(health_handler, NULL);
  if(!s_health_available) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
}

bool health_is_available() {
  return s_health_available;
}

int health_get_uptime_seconds() {
  return (time(NULL) - s_launch_time);
}

int health_get_metric_sum(HealthMetric metric) {
  return (int)get_metric_if_available(metric);
}

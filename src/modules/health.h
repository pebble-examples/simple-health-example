#pragma once

#include <pebble.h>

#include "../windows/main_window.h"

void health_init();

bool health_is_available();

int health_get_metric_sum(HealthMetric metric);

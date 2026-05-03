/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Start a measurement
 *
 * Return true if measurement successfully started
 */
bool start_measurement();

/* Stop a measurement
 *
 * Return false if measurement successfully stopped
 */
bool stop_measurement();

/* Toggle the measurement state
 */
void toggle_measurement();

/* Return true if equipment is ready */
bool is_measurement_ready();
/* Call when measurement ready */
void toggle_measurement_ready();
/* Return true if measurement is on  */
bool is_measurement_on();

#ifdef __cplusplus
}
#endif

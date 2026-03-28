#pragma once

#include <stdbool.h>

/* Start a measurement
 *
 * Return true if measurement successfully started
 */
bool start_measurement();

/* Stop a measurement
 *
 * Return false if measurement successfully stopped
 */
bool end_measurement();

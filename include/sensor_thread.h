/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/zbus/zbus.h>

// The zbus message containing the raw data
struct sensor_data_msg {
    struct sensor_value uv;
    bool ok;
};

ZBUS_CHAN_DECLARE(sensor_data_chan);
ZBUS_OBS_DECLARE(sensor_thread_sub);

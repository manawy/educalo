/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/drivers/sensor.h>

// The zbus message containing the raw data
struct sensor_data_msg {
    struct sensor_value uv;
    bool ok;
};

// Zbus msg to set an attribute
struct sensor_attr_msg {
    enum sensor_attribute attr;
    struct sensor_value val;
};

void get_current_fsr(struct sensor_value *val);


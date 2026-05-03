/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

struct processing_thread_msg {
    bool to_save;
    int32_t value;
    int64_t timestamp;
};
//
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

ZBUS_OBS_DECLARE(sensor_thread_sub);
ZBUS_OBS_DECLARE(processing_thread_sub);
ZBUS_OBS_DECLARE(datalogger_thread_sub);

ZBUS_CHAN_DECLARE(measurement_ready_chan);
ZBUS_CHAN_DECLARE(measurement_busy_chan);
ZBUS_CHAN_DECLARE(start_measure_chan);
ZBUS_CHAN_DECLARE(end_measure_chan);

ZBUS_CHAN_DECLARE(start_trigger_chan);
ZBUS_CHAN_DECLARE(sensor_data_chan);
ZBUS_CHAN_DECLARE(processing_thread_chan);
ZBUS_CHAN_DECLARE(sensor_attr_chan);
ZBUS_CHAN_DECLARE(end_onebeat_chan) ;

#ifdef __cplusplus
} // extern C
#endif

/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "processing_thread.h"
#include "zbus_channels.h"
#include "sensor_thread.h"
#include "zephyr/drivers/sensor.h"

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(core_thread, LOG_LEVEL_INF);

static int64_t current_fsr;

static void check_gain(int64_t value) 
{
   int64_t requested_fsr = 0; 
    if (value < 171000) {
        requested_fsr = 256000;
    } else if (value < 426000) {
        requested_fsr = 512000;
    } else if (value < 683000) {
        requested_fsr = 1024000;
    } else {
        requested_fsr = 2048000;
    };
    if (requested_fsr == current_fsr) {
        return;
    }

    struct sensor_attr_msg msg;
    msg.attr = SENSOR_ATTR_GAIN;
    LOG_INF("Request new gain: %lld", requested_fsr);
    sensor_value_from_micro(&msg.val, requested_fsr);
    int rc = zbus_chan_pub(&sensor_attr_chan, &msg, K_MSEC(100));
    if (rc == 0) {
        current_fsr = requested_fsr;
    }
}


// Process one sample and notify/publish accordingly
static void process_one(struct sensor_data_msg *msg)
{
    static int64_t buffer = 0;
    static int counter = 0;

    if (msg->ok != true) {
        zbus_chan_notify(&end_onebeat_chan, K_MSEC(50));
    }

    struct processing_thread_msg processed_data = {.to_save=0, .value=0};
    int64_t val = sensor_value_to_micro(&msg->uv);
    buffer += val;

    check_gain(val);

    if (++counter < CONFIG_OVERSAMPLING) {
        // nothing to do - wait for nex sample
        zbus_chan_pub(&end_onebeat_chan, &counter, K_MSEC(50));
        return;
    }

    // data is ready to be send to datalogger
    processed_data.value = buffer/CONFIG_OVERSAMPLING;
    processed_data.to_save = msg->ok;
    processed_data.timestamp = k_uptime_get();

    buffer = 0;
    counter = 0;
    zbus_chan_pub(&processing_thread_chan, &processed_data, K_MSEC(50));
    return;
}

void init_current_fsr() {
    struct sensor_value val;
    get_current_fsr(&val);
    current_fsr = sensor_value_to_micro(&val);
}

void processing_thread()
{
    const struct zbus_channel* chan;
    struct sensor_data_msg msg;

    init_current_fsr();

    while(1) {
        zbus_sub_wait(&processing_thread_sub, &chan, K_FOREVER);

        int err = zbus_chan_read(&sensor_data_chan, &msg, K_MSEC(10));
        if (err) {
            LOG_WRN("Could not read data channel. Error code: %d", err);
            continue;;
        }
        process_one(&msg);
    }
}

K_THREAD_DEFINE(processing_thread_id,
                2048,
                processing_thread,
                NULL, NULL, NULL,
                CONFIG_PROCESSING_THREAD_PRIORITY, 0,
                1000);




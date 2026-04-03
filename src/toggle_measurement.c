/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "toggle_measurement.h"
#include "sensor_thread.h"
#include "leds_interface.h"
#include "zephyr/zbus/zbus.h"

#define HEARTBEAT K_MSEC(CONFIG_HEARTBEAT_MSEC)

LOG_MODULE_REGISTER(toggle_measurement, CONFIG_LOG_DEFAULT_LEVEL);

ZBUS_LISTENER_DEFINE(leds_busy_listener, listener_ledbusy_set);
ZBUS_OBS_DECLARE(datalogger_thread_sub);

ZBUS_CHAN_DEFINE(start_trigger_chan,
                uint8_t,
                NULL, NULL,
                ZBUS_OBSERVERS(leds_busy_listener, sensor_thread_sub),
                0);

ZBUS_CHAN_DEFINE(start_measure_chan,
                 uint8_t,
                 NULL, NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub),
                 0);

ZBUS_CHAN_DEFINE(end_measure_chan,
                 uint8_t,
                 NULL, NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub),
                 0);

void run_trigger() {
    LOG_DBG("run trigger");
    zbus_chan_notify(&start_trigger_chan, K_MSEC(100));
}


K_TIMER_DEFINE(heartbeat_timer, run_trigger, NULL);



bool start_measurement() {
    LOG_PRINTK(" ---- Start measurement ---- \n");
    k_timer_start(&heartbeat_timer, HEARTBEAT, HEARTBEAT);
    zbus_chan_notify(&start_measure_chan, K_MSEC(100));
    return true;
}

bool end_measurement() {
    LOG_PRINTK(" ---- Stop measurement ---- \n");
    k_timer_stop(&heartbeat_timer);
    zbus_chan_notify(&end_measure_chan, K_MSEC(100));
    return false;
}




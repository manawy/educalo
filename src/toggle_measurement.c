/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "toggle_measurement.h"
#include "leds_interface.h"
#include "zbus_channels.h"

#define HEARTBEAT K_MSEC(CONFIG_HEARTBEAT_MSEC)

LOG_MODULE_REGISTER(toggle_measurement, CONFIG_LOG_DEFAULT_LEVEL);


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




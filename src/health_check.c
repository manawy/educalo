/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include <zephyr/drivers/sensor.h>

LOG_MODULE_REGISTER(health_check, CONFIG_LOG_DEFAULT_LEVEL);

static int boardt_init(const struct device* const tdev) {
    if (!device_is_ready(tdev)) {
        LOG_ERR("Board temperature sensor not ready");
        return -1;
    }
    return 0;
}

static const struct device *const tboard =
    DEVICE_DT_GET(DT_ALIAS(boardt));

void health_check_thread() {


    boardt_init(tboard);

    while (1) {
        k_sleep(K_SECONDS(CONFIG_HEALTHCHECK_BEAT_SECONDS));

        struct sensor_value temp;
        int ret = sensor_sample_fetch(tboard);
        if (ret !=0) {
            LOG_ERR("Failed reading board temperature");
        } else {
            sensor_channel_get(tboard, SENSOR_CHAN_AMBIENT_TEMP, &temp);
            LOG_PRINTK("Board temperature %.2f\n",
                    sensor_value_to_float(&temp));
        }
    }
}

K_THREAD_DEFINE(health_check_id,
                1024,
                health_check_thread,
                NULL, NULL, NULL,
                15, 0,
                1000);

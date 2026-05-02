/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

/*  This module create a thread to regularly check 
 *  and report the status of the thread;
 *
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include <zephyr/drivers/sensor.h>

LOG_MODULE_REGISTER(health_check, CONFIG_LOG_DEFAULT_LEVEL);


/* Track the status of the board
 */
class HealthCheck { 
public:

    //HealthCheck(const struct device* const tboard);
    HealthCheck();

    // Return 0 if all healthcheck devices ready
    int is_ready();

    // Read the relevant sensors
    int read();

    // Return the board temperature
    //
    // Must be read using HealthCheck::read() first
    double board_temperature();

private:
    bool m_is_init;
    struct sensor_value m_buffer_temp;
    //const struct device* const mTboard;
    static constexpr const struct device *const mTboard = DEVICE_DT_GET(DT_ALIAS(boardt));
};

HealthCheck::HealthCheck(): //(const struct device* const tboard):
    m_is_init(false)
{
    m_buffer_temp.val1 = 0;
    m_buffer_temp.val2 = 0;
}

int HealthCheck::is_ready() {
    if (!device_is_ready(mTboard)) {
        m_is_init = false;
        return -EIO;
    }
    m_is_init = true;
    return 0;
}

int HealthCheck::read() {
    if (!m_is_init)
        return -EIO;
    int ret = sensor_sample_fetch(mTboard);
    if (ret == 0)
        sensor_channel_get(mTboard, SENSOR_CHAN_AMBIENT_TEMP, &m_buffer_temp);
    return ret;
}

double HealthCheck::board_temperature() {
    if (!m_is_init)
        return -1.0;
    return sensor_value_to_double(&m_buffer_temp);
}

void health_check_thread() {
    LOG_MODULE_DECLARE(health_check);

    HealthCheck checker;
    int ret = checker.is_ready();
    if (ret != 0) {
        LOG_ERR("Failed to initialize health check !");
    }

    while (1) {
        int ret = checker.read();
        if (ret !=0) {
            LOG_ERR("Failed reading board temperature");
        } else {
            LOG_PRINTK("Board temperature : %.2f Celsius.\n", checker.board_temperature());
        }

        k_sleep(K_SECONDS(CONFIG_HEALTHCHECK_BEAT_SECONDS));
    }
}

K_THREAD_DEFINE(health_check_id,
                1024,
                health_check_thread,
                NULL, NULL, NULL,
                15, 0,
                1000);

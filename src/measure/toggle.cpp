/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "measure/toggle.h"
#include "zbus_channels.h"

LOG_MODULE_REGISTER(toggle, CONFIG_LOG_DEFAULT_LEVEL);

constexpr k_timeout_t HEARTBEAT = K_MSEC(CONFIG_HEARTBEAT_MSEC);
constexpr k_timeout_t zbus_timeout = K_MSEC(50);

class MeasurementState
{
public:
    MeasurementState():
        measurement_ready(false),
        measurement_on(false)
    {

        k_timer_init(&m_heartbeat, run_trigger, NULL);
    }

    static void run_trigger(struct k_timer*) {
        LOG_INF("Triggered !");
        zbus_chan_notify(&start_trigger_chan, zbus_timeout);
    }

    bool start_measurement();
    bool stop_measurement();

    bool is_measurement_ready() const {
        return measurement_ready;
    }

    bool is_measurement_on() const {
        return measurement_on;
    }

    bool toggle_measurement() {
        if (is_measurement_on()) 
            return stop_measurement();
        return start_measurement();
    };

    void toggle_measurement_ready() {
        measurement_ready = !measurement_ready;
    }


private:
    bool measurement_ready;
    bool measurement_on;
    struct k_timer m_heartbeat;
};


bool MeasurementState::start_measurement() 
{
    if (!is_measurement_ready() || is_measurement_on()) {
        return false;
    }
    zbus_chan_notify(&start_measure_chan, zbus_timeout);
    k_timer_start(&m_heartbeat, HEARTBEAT, HEARTBEAT);
    measurement_ready = false;
    measurement_on = true;
    return measurement_on;
}

bool MeasurementState::stop_measurement() 
{
    if (!is_measurement_on()) {
        return false;
    }
    k_timer_stop(&m_heartbeat);
    zbus_chan_notify(&end_measure_chan, zbus_timeout);
    measurement_on = false;
    return measurement_on;
}

// ---- API -------

MeasurementState mst;

bool start_measurement() {
    return mst.start_measurement();
}
bool stop_measurement() {
    return mst.stop_measurement();
}
bool is_measurement_on() {
    return mst.is_measurement_on();
}
void toggle_measurement() {
    mst.toggle_measurement();
}
bool is_measurement_ready() {
    return mst.is_measurement_ready();
}
void toggle_measurement_ready() {
    return mst.toggle_measurement_ready();
}


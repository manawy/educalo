/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */
#include <zephyr/zbus/zbus.h>

#include "zbus_channels.h"
#include "app_info.h"
#include "interface/leds.h"
#include "measure/toggle.h"

ZBUS_SUBSCRIBER_DEFINE(sensor_thread_sub, 4);
ZBUS_SUBSCRIBER_DEFINE(processing_thread_sub, 4);
ZBUS_SUBSCRIBER_DEFINE(datalogger_thread_sub, 4);
ZBUS_LISTENER_DEFINE(leds_busy_listener, listener_ledbusy_set);
ZBUS_LISTENER_DEFINE(leds_on_listener, listener_ledon_set);
ZBUS_LISTENER_DEFINE(set_measurement_ready_listener, toggle_measurement_ready);


ZBUS_CHAN_DEFINE(
    app_info_chan,
    struct app_info_msg,
    NULL,
    NULL,
    ZBUS_OBSERVERS_EMPTY,
    ZBUS_MSG_INIT(.name = "CaloBus",
                  .hardware_version={0, 1})
);

// -- Toggle measurement

ZBUS_CHAN_DEFINE(measurement_ready_chan,
                 uint8_t,
                 NULL, NULL,
                 ZBUS_OBSERVERS(set_measurement_ready_listener, leds_on_listener),
                 0);

ZBUS_CHAN_DEFINE(start_measure_chan,
                 uint8_t,
                 NULL, NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub, leds_on_listener),
                 0);

ZBUS_CHAN_DEFINE(end_measure_chan,
                 uint8_t,
                 NULL, NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub),
                 0);

// -- Measure sequence ---

ZBUS_CHAN_DEFINE(start_trigger_chan,
                uint8_t,
                NULL, NULL,
                ZBUS_OBSERVERS(leds_busy_listener, sensor_thread_sub),
                ZBUS_MSG_INIT(0)
                 );

ZBUS_CHAN_DEFINE(sensor_data_chan,
                 struct sensor_data_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(processing_thread_sub),
                 ZBUS_MSG_INIT(0)
                 );

ZBUS_CHAN_DEFINE(processing_thread_chan,
                 struct processing_thread_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub),
                 ZBUS_MSG_INIT(0)
                 );

ZBUS_CHAN_DEFINE(sensor_attr_chan,
                 struct sensor_attr_msg,
                 NULL, NULL,
                 ZBUS_OBSERVERS(sensor_thread_sub),
                 ZBUS_MSG_INIT(0, {0,0})
                 );

ZBUS_CHAN_DEFINE(end_onebeat_chan,
                 int,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(leds_busy_listener),
                 ZBUS_MSG_INIT(0)
                 );

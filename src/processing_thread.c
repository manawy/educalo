#include "processing_thread.h"
#include "sensor_thread.h"

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(core_thread, LOG_LEVEL_INF);

ZBUS_CHAN_DECLARE(sensor_data_chan);

ZBUS_SUBSCRIBER_DEFINE(processing_thread_sub, 4);
ZBUS_OBS_DECLARE(datalogger_thread_sub);

ZBUS_CHAN_DEFINE(processing_thread_chan,
                 struct processing_thread_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(datalogger_thread_sub),
                 ZBUS_MSG_INIT(0)
                 );

void processing_thread() {
    const struct zbus_channel* chan;

    struct processing_thread_msg processed_data = {.to_save=0, .value=0};

    struct sensor_data_msg msg;

    while(1) {
        zbus_sub_wait(&processing_thread_sub, &chan, K_FOREVER);

        int err = zbus_chan_read(&sensor_data_chan, &msg, K_MSEC(100));
        if (err) {
            LOG_WRN("Could not read data channel. Error code: %d", err);
        } else {
            processed_data.value = msg.uv;
        }
        processed_data.to_save = msg.ok;

        zbus_chan_pub(&processing_thread_chan, &processed_data, K_MSEC(100));
    }
}

K_THREAD_DEFINE(processing_thread_id,
                2048,
                processing_thread,
                NULL, NULL, NULL,
                15, 0,
                1000);




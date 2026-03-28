#include "processing_thread.h"
#include "leds_interface.h"
#include "filesystem.h"

#include <stdint.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>

LOG_MODULE_REGISTER(datalogger_thread, LOG_LEVEL_INF);

ZBUS_SUBSCRIBER_DEFINE(datalogger_thread_sub, 4);

ZBUS_LISTENER_DEFINE(leds_busy_end_listener, listener_ledbusy_set);

ZBUS_CHAN_DEFINE(datalogger_thread_chan,
                 int,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(leds_busy_end_listener),
                 ZBUS_MSG_INIT(0)
                 );



void datalogger_thread(void) {
    const struct zbus_channel* chan;

    struct processing_thread_msg processed_data;
    int return_code = 0;

    struct fs_file_t file;
    fs_file_t_init(&file);

    bool file_is_open = false;

    while(1) {
        zbus_sub_wait(&datalogger_thread_sub, &chan, K_FOREVER);

        if (!file_is_open) {
            char path[128];
            get_sd_timed_path(path, "measure");

            int rc = fs_open(&file, path,
                    FS_O_CREATE | FS_O_WRITE | FS_O_APPEND); 
            if (rc != 0) {
                LOG_ERR("Failed to open file %s", path);
            } else {
                file_is_open = true;
            }
        }

        int err = zbus_chan_read(&processing_thread_chan,
                                 &processed_data, K_MSEC(100));
        if (err) {
            LOG_WRN("Could not read processed data channel. Error code: %d", err);
        }
        return_code = err;
        printk("Processed data: %d\n", processed_data.value);
        if (file_is_open) {
            char buf[128];
            sprintf(buf, "%"PRId32"\n", processed_data.value);
        }

        zbus_chan_pub(&datalogger_thread_chan, &return_code, K_MSEC(100));
    }
}

K_THREAD_DEFINE(datalogger_thread_id,
                2048,
                datalogger_thread,
                NULL, NULL, NULL,
                16, 0,
                1000);



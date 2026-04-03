/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "processing_thread.h"
#include "leds_interface.h"
#include "toggle_measurement.h"
#include <zephyr/drivers/rtc.h>

#include <stdint.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#ifdef CONFIG_SDLOGGING
#include "filesystem.h"
#include <zephyr/fs/fs.h>
#endif

LOG_MODULE_REGISTER(datalogger_thread, LOG_LEVEL_INF);

ZBUS_SUBSCRIBER_DEFINE(datalogger_thread_sub, 4);

ZBUS_LISTENER_DEFINE(leds_busy_end_listener, listener_ledbusy_set);

ZBUS_CHAN_DEFINE(end_onebeat_chan,
                 int,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(leds_busy_end_listener),
                 ZBUS_MSG_INIT(0)
                 );


#ifdef CONFIG_SDLOGGING
struct fs_file_t file;
bool file_is_open = false;
#endif

static int64_t timestamp_0;

const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

static int datalog_one() {

    struct processing_thread_msg processed_data;
    int return_code = 0;

    int err = zbus_chan_read(&processing_thread_chan,
                             &processed_data, K_MSEC(98));
    if (err) {
        LOG_WRN("Could not read processed data channel. Error code: %d", err);
    }
    return_code = err;
    LOG_PRINTK("%lld,%d\n", processed_data.timestamp - timestamp_0, processed_data.value);

    #ifdef CONFIG_SDLOGGING
    if (file_is_open) {
        char buf[128];
        sprintf(buf, "%lld,%"PRId32"\n",
                processed_data.timestamp-timestamp_0,
                processed_data.value);
        fs_write(&file, buf, strlen(buf));
    }
    #endif // CONFIG_SDLOGGING
    return return_code;

}

static int datalog_setup() {
    struct rtc_time tm;
    int ret = rtc_get_time(rtc, &tm);
    timestamp_0 = k_uptime_get();
    if (ret <0) {
        LOG_ERR("Cannot get RTC time !");
    }
    LOG_PRINTK("#Initial timestamp: ");
    LOG_PRINTK("%02d%02d%02d_%02d%02d%02d ",
                tm.tm_year-100,
                tm.tm_mon+1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);
    LOG_PRINTK("%lld\n", timestamp_0);

    #ifdef CONFIG_SDLOGGING
    fs_file_t_init(&file);
    if (!file_is_open) {
        char path[128];
        get_sd_timed_path(path, "m", &tm, ".dat");

        int rc = fs_open(&file, path, FS_O_CREATE | FS_O_RDWR | FS_O_APPEND); 
        if (rc != 0) {
            LOG_ERR("Failed to open file %s", path);
        } else {
            file_is_open = true;
        }
    }
    #endif
    return 0;
}

static int datalog_end() {
    if (file_is_open) {
        fs_close(&file);
        file_is_open = false;
    }
    return 0;
}

void datalogger_thread(void) {
    const struct zbus_channel* chan;
    while(1) {
        zbus_sub_wait(&datalogger_thread_sub, &chan, K_FOREVER);

        if (&processing_thread_chan == chan){
            int rc = datalog_one();
            zbus_chan_pub(&end_onebeat_chan, &rc, K_MSEC(98));
        } else if (&start_measure_chan == chan) {
            datalog_setup();
        } else if (&end_measure_chan == chan) {
            datalog_end();
        }
    }
}

K_THREAD_DEFINE(datalogger_thread_id,
                2048,
                datalogger_thread,
                NULL, NULL, NULL,
                5, 0,
                1000);



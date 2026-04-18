/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 * datalogger_thread
 * -----------------
 *
 *  The thread responsible for datalogging
 *
 *  Datalogging occurs on:
 *      On LOG_PRINTK output
 *      SD card: /SD:/m<date><time>.dat if CONFIG_SDLOGGING
 *
 * This thread is responsible for handling file creation, opening and closing
 */

#include "measure/processor.h"
#include "zbus_channels.h"

#include <stdint.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>

#ifdef CONFIG_SDLOGGING
#include "filesystem.h"
#include <zephyr/fs/fs.h>
#endif

LOG_MODULE_REGISTER(datalogger_thread, LOG_LEVEL_INF);

// ---- Static global variables ------------------

// File handlers
#ifdef CONFIG_SDLOGGING
    static struct fs_file_t file;
    static bool file_is_open = false;
#endif

// Time at the start of the measurement
static int64_t timestamp_0;

// The real time clock
static const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

// ---- helper functions --------------------------


// Log one timepoint onto the sd card
static int datasdlog_one(struct processing_thread_msg* processed_data) {
    if (!file_is_open) {
        LOG_DBG("Datalogging file not open - no logging");
        return -EIO;
    }
    char buf[128];
    sprintf(buf, "%lld,%"PRId32"\n",
            processed_data->timestamp-timestamp_0,
            processed_data->value);
    return fs_write(&file, buf, strlen(buf));
}

// Log one data-point
static int datalog_one() {
    struct processing_thread_msg processed_data;
    int return_code =  zbus_chan_read(&processing_thread_chan,
                             &processed_data, K_MSEC(98));
    if (return_code != 0) {
        LOG_WRN("Could not read processed data channel. Error code: %d", return_code);
    }
    LOG_PRINTK("%lld,%d\n", processed_data.timestamp - timestamp_0, processed_data.value);

    #ifdef CONFIG_SDLOGGING
        return_code = datasdlog_one(&processed_data);
    #endif // CONFIG_SDLOGGING
    return return_code;
}

// Setup the datlogging at the start of the measurement
static int datalog_setup() {
    struct rtc_time tm;
    int ret = rtc_get_time(rtc, &tm);
    timestamp_0 = k_uptime_get();
    if (ret <0) {
        LOG_ERR("Cannot get RTC time ! return code : %d", ret);
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

// Clean the datalogging environment at the end of the measurement
static int datalog_end() {
    if (file_is_open) {
        fs_close(&file);
        file_is_open = false;
    }
    LOG_DBG("Datalogging end");
    return 0;
}

// ----- Thread ----------------------------------------

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
                CONFIG_DATALOGGER_THREAD_PRIORITY, 0,
                1000);



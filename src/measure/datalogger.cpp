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

#include "measure/toggle.h"
#include "zbus_channels.h"
#include "calo_time.h"
#include "zephyr/zbus/zbus.h"

#include <stdint.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>

#ifdef CONFIG_SDLOGGING
#include "filesystem.h"
#include <zephyr/fs/fs.h>
#endif

LOG_MODULE_REGISTER(datalogger_thread, CONFIG_LOG_DEFAULT_LEVEL);

template <class Derived>
class IDatalogger
{
public:
    IDatalogger():
        m_timestamp_0(0)
    {}

    int start(const int64_t& timestamp_0) {
        set_t0(timestamp_0);
        return static_cast<Derived*>(this)->start_measurement();
    }

    void set_t0(const int64_t& timestamp_0) {
        m_timestamp_0 = timestamp_0;
    }

    const int64_t& get_t0() {
        return m_timestamp_0;
    }

    int stop() {
        return static_cast<Derived*>(this)->stop_measurement();
    }

    int log(const struct processing_thread_msg* const data) {
        return static_cast<Derived*>(this)->log_one(data);
    }

private:
    int64_t m_timestamp_0;

};

/* The console logger
 *
 * Print each measurement to the console
 */
class ConsoleDataLogger: public IDatalogger<ConsoleDataLogger>
{
public:
    ConsoleDataLogger() = default;

    int start_measurement() {
        LOG_PRINTK("------ Start measurement ------\n");
        return 0;
    };
    int stop_measurement() {
        LOG_PRINTK("------ Stop  measurement ------\n");
        return 0;
    };
    int log_one(const struct processing_thread_msg* const data) {
        LOG_PRINTK("%lld,%d\n",
                data->timestamp - get_t0(),
                data->value);
        return 0;
    }
};

/* The file logger
 *
 * Save each measurement in a data file
 */
class FileDataLogger: public IDatalogger<FileDataLogger>
{
public:
    FileDataLogger():
        m_file_open(false)
    {}

    ~FileDataLogger();

    int start_measurement();

    int stop_measurement();

    int log_one(const struct processing_thread_msg* const data);

private:
    bool open_file();
    bool close_file();
    void write_header();

    bool m_file_open;
    struct fs_file_t m_file;
    char m_buf[128];
};


FileDataLogger::~FileDataLogger()
{
    close_file();
}

int FileDataLogger::start_measurement()
{
    if (!open_file()) 
        return -EIO;
    write_header();
    return 0;
}

void FileDataLogger::write_header()
{
    constexpr char buf[] = "# timestamp, voltage\n";
    fs_write(&m_file, buf, strlen(buf));
}

int FileDataLogger::stop_measurement()
{
    if (close_file())
        return 0;
    else
        return -EIO;
}


bool FileDataLogger::open_file()
{

    if (m_file_open) return m_file_open;

    char buf[128];
    struct tm tm;
    get_time(&tm);

    fs_file_t_init(&m_file);
    get_sd_timed_path(buf, "m", &tm, ".dat");

    int rc = fs_open(&m_file, buf, FS_O_CREATE | FS_O_RDWR | FS_O_APPEND); 
    if (rc != 0) {
        LOG_ERR("Failed to open file %s", buf);
    } else {
       m_file_open = true;
    }

    return m_file_open;
}

bool FileDataLogger::close_file()
{
    if (m_file_open)
    {
        fs_close(&m_file);
        m_file_open = false;
    }
    return m_file_open;
}

int FileDataLogger::log_one(const struct processing_thread_msg* const data)
{
    if (!m_file_open)
        return -EIO;

    snprintf(m_buf, 128, "%lld,%d\n",
        data->timestamp - get_t0(),
        data->value);
    int ret = fs_write(&m_file, &m_buf, strlen(m_buf));

    if (ret <0)
    {
        LOG_ERR("failed to write to file");
        return -EIO;
    }
    return 0;
}

/* combine the loggers
 *
 * Uses fold expressions (c++17 to iterate over all loggers supplied as types)
 */
template <typename... Loggers>
class CombinedLoggers: public Loggers...
{
public:
    int start(const int64_t& timestamp_0) {
        return (Loggers::start(timestamp_0) + ...);
    }

    int stop() {
        return (Loggers::stop() + ...);
    }

    int log(const struct processing_thread_msg* const data) {
        return (Loggers::log(data) + ...);
    }
};

#ifdef CONFIG_SDLOGGING
static CombinedLoggers<FileDataLogger, ConsoleDataLogger> Logger;
#else
static CombinedLoggers<ConsoleDataLogger> Logger;
#endif

// ----- Thread ----------------------------------------

void datalogger_thread(void) {
    const struct zbus_channel* chan;

    while(1) {
        zbus_sub_wait(&datalogger_thread_sub, &chan, K_FOREVER);

        if (&processing_thread_chan == chan){
            struct processing_thread_msg processed_data;
            zbus_chan_read(&processing_thread_chan,
                             &processed_data, K_MSEC(50));
            int rc = Logger.log(&processed_data);
            zbus_chan_pub(&end_onebeat_chan, &rc, K_MSEC(50));
        } else if (&start_measure_chan == chan) {
            auto timestamp_0 = k_uptime_get();
            Logger.start(timestamp_0);
        } else if (&end_measure_chan == chan) {
            Logger.stop();
            zbus_chan_notify(&measurement_ready_chan, K_MSEC(50));
        }
    }
}

K_THREAD_DEFINE(datalogger_thread_id,
                2048,
                datalogger_thread,
                NULL, NULL, NULL,
                CONFIG_DATALOGGER_THREAD_PRIORITY, 0,
                1000);



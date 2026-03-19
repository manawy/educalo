/*
 * Calo firmware
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 *
 */

#include "app_info.h"
#include "leds_interface.h"

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/input/input.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

ZBUS_CHAN_DECLARE(app_info_chan, sensor_data_chan);

ZBUS_OBS_DECLARE(sensor_thread_sub);

ZBUS_LISTENER_DEFINE(leds_busy_listener, listener_ledbusy_set);

ZBUS_CHAN_DEFINE(start_trigger_chan,
                uint8_t,
                NULL, NULL,
                //ZBUS_OBSERVERS(sensor_thread_sub),
                ZBUS_OBSERVERS(leds_busy_listener, sensor_thread_sub),
                0);

void run_trigger() {
    LOG_DBG("run trigger");
    zbus_chan_notify(&start_trigger_chan, K_MSEC(100));
}

K_TIMER_DEFINE(heartbeat_timer, run_trigger, NULL);

// --- Console

void console_init() {
    #if DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart)
        const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
        uint32_t dtr = 0;

        /* Poll if the DTR flag was set */
        while (!dtr) {
            uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
            /* Give CPU resources to low priority threads. */
            k_sleep(K_MSEC(100));
        }
    #endif
}

bool is_ready = false;
bool MeasurementOn = false;

static void btn_toggle_measurement(struct input_event *evt, void *user_data) {
    if (!is_ready) {
        return;
    }
    if ((evt->code == INPUT_KEY_0) & (evt->value == 0)) {
        if (!MeasurementOn) {
            LOG_PRINTK(" ---- Start measurement ---- \n");
            k_timer_start(&heartbeat_timer, K_SECONDS(1), K_SECONDS(1));
            MeasurementOn = true;
        } else {
            LOG_PRINTK(" ---- Stop measurement ---- \n");
            k_timer_stop(&heartbeat_timer);
            MeasurementOn = false;
        }
    }
}
INPUT_CALLBACK_DEFINE(NULL, btn_toggle_measurement, NULL);

// ---- Main

int main() {
    console_init();
    int ret = ledbusy_init();
    if (ret <0) {
        LOG_ERR("Error led init");
    }

    struct app_info_msg* app_info = 
        (struct app_info_msg*) zbus_chan_const_msg(&app_info_chan);

    LOG_PRINTK("-----------------------------\n");
    LOG_PRINTK("%s\n", app_info->name);
    LOG_PRINTK(" firmware %d.%d\n", 
            app_info->firmware_version.major,
            app_info->firmware_version.minor);
    LOG_PRINTK(" hardware %d.%d\n", 
            app_info->hardware_version.major,
            app_info->hardware_version.minor);
    LOG_PRINTK("-----------------------------\n");

    ledon_start();
    is_ready = true;

    return 0;
}

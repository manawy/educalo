/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "app_info.h"
#include "interface/leds.h"
#include "measure/toggle.h"
#include "filesystem.h"
#include "calo_time.h"
#include "time.h"

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);


//      Console
//---------------------

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


void rtc_init() {

}

//       Main
//------------------


int main() {
    init_time();

    console_init();
    LOG_INF("Start main");
    int ret = ledbusy_init();
    if (ret <0) {
        LOG_ERR("Error led init");
    }

    #ifdef CONFIG_SDLOGGING
    LOG_INF("Initializing SD card");
    ret = init_sd_card();
    if (ret < 0) {
        LOG_ERR("No SD card");
    }
    LOG_INF("Init SD card done");
    #endif

    struct app_info_msg* app_info = 
        (struct app_info_msg*) zbus_chan_const_msg(&app_info_chan);

    LOG_PRINTK("-----------------------------\n");
    LOG_PRINTK("%s\n", app_info->name);
    LOG_PRINTK(" firmware " APP_VERSION_STRING "\n");
    LOG_PRINTK(" hardware %d.%d\n", 
            app_info->hardware_version.major,
            app_info->hardware_version.minor);
    LOG_PRINTK("-----------------------------\n");

    set_measurement_ready();

    return 0;
}

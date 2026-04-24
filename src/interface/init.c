/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "app_info.h"
#include "interface/leds.h"
#include "interface/shell.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(init_interface, CONFIG_LOG_DEFAULT_LEVEL);

void init_interface() {
    console_init();
    LOG_INF("Start main");
    int ret = ledbusy_init();
    if (ret <0) {
        LOG_ERR("Error led init");
    }

    struct app_info_msg* app_info = 
        (struct app_info_msg*) zbus_chan_const_msg(&app_info_chan);

    LOG_PRINTK("-----------------------------\n");
    LOG_PRINTK("%s\n", app_info->name);
    LOG_PRINTK(" firmware " APP_VERSION_STRING "\n");
    LOG_PRINTK(" hardware %d.%d\n", 
            app_info->hardware_version.major,
            app_info->hardware_version.minor);
    LOG_PRINTK("-----------------------------\n");
}

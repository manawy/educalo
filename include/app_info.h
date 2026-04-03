/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/zbus/zbus.h>
#include <stdint.h>

/* The information stored in app_info channel of zbus
 *
 *  Generic info about the firmware
 *
 */
struct app_info_msg {
    const char name[32];
    const struct  {
        uint8_t major;
        uint8_t minor;
    } firmware_version;
    const struct {
        uint8_t major;
        uint8_t minor;
    } hardware_version;
};

ZBUS_CHAN_DECLARE(app_info_chan);

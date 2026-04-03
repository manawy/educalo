/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "app_info.h"

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(
    app_info_chan,
    struct app_info_msg,
    NULL,
    NULL,
    ZBUS_OBSERVERS_EMPTY,
    ZBUS_MSG_INIT(.name = "CaloBus",
                  .firmware_version={0, 1},
                  .hardware_version={0, 1})
);

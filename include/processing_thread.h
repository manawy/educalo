/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/zbus/zbus.h>

struct processing_thread_msg {
    bool to_save;
    int32_t value;
    int64_t timestamp;
};



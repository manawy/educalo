#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/zbus/zbus.h>

struct processing_thread_msg {
    bool to_save;
    int32_t value;
};

ZBUS_CHAN_DECLARE(processing_thread_chan);


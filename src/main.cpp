/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "calo_time.h"
#include "filesystem.h"
#include "interface/init.h"
#include "zbus_channels.h"

int main() {
    init_time();
    init_filesystem();
    init_interface();
    zbus_chan_notify(&measurement_ready_chan, K_MSEC(50));
    return 0;
}

/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "calo_time.h"
#include "filesystem.h"
#include "interface/init.h"
#include "measure/toggle.h"

int main() {
    init_time();
    init_filesystem();
    init_interface();
    set_measurement_ready();
    return 0;
}

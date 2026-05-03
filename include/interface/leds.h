/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/zbus/zbus.h>

int leds_init();
void listener_ledbusy_set(const struct zbus_channel *chan);
void listener_ledon_set(const struct zbus_channel *chan);


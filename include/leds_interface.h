/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/zbus/zbus.h>

int ledbusy_init();
void listener_ledbusy_set(const struct zbus_channel *chan);

int ledon_init();
void ledon_start();

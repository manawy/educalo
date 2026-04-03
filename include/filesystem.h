/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_POINT "/SD:"

struct rtc_time;

// SD Card initialization
//
// Check if sd card is in and mount filesystem
int init_sd_card();

// SD Card de-initialization
int de_init_sd_card();

// Return a full path at the root of the SD card
void get_sd_full_path(char *path, const char* filename);

// Return a path with a timed suffix
void get_sd_timed_path(char *path,
                       const char* prefix,
                       struct rtc_time* tm,
                       const char* suffix
                      );

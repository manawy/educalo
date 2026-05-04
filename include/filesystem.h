/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <time.h>

#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_POINT "/SD:"

#ifdef __cplusplus
extern "C" {
#endif


// Return a full path at the root of the SD card
void get_sd_full_path(char *path, const char* filename);

// Return a path with a timed suffix
void get_sd_timed_path(char *path,
                       const char* prefix,
                       struct tm* tm,
                       const char* suffix
                      );

#ifdef __cplusplus
}
#endif

#pragma once

#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_POINT "/SD:"

// SD Card initialization
//
// Check if sd card is in and mount filesystem
int init_sd_card();

// SD Card de-initialization
int de_init_sd_card();

// Return a full path at the root of the SD card
void get_sd_full_path(char *path, const char* filename);

// Return a path with a timed suffix
void get_sd_timed_path(char *path, const char* prefix);

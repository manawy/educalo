/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include "filesystem.h"
#include <iterator>
#include <string.h>
#include <zephyr/kernel.h>
#include "calo_time.h"

#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>

#include <string_view>

LOG_MODULE_REGISTER(filesystem, LOG_LEVEL_INF);

static constexpr std::string_view disk_mount_pt = DISK_MOUNT_POINT;
static constexpr std::string_view disk_name = DISK_DRIVE_NAME;

#ifdef CONFIG_FAT_FILESYSTEM_ELM
#include <ff.h>

struct SDFATFSMount
{
    FATFS fat_fs;
    struct fs_mount_t mp_sd = {
        .type = FS_FATFS,
        .fs_data = &fat_fs,
    };
    int FS_RET_OK = FR_OK;

    void set_mount_point() {
        mp_sd.mnt_point = disk_mount_pt.data();
    }
};

typedef SDFATFSMount SDMount;

#elifdef CONFIG_FILE_SYSTEM_LITTLEFS
#include <zephyr/fs/littlefs.h>

struct SDLittleFSMount
{
    struct fs_littlefs lfsfs;
    struct fs_mount_t mp_sd = {
        .type = FS_LITTLEFS,
        .fs_data = &lfsfs,
        .flags = FS_MOUNT_FLAG_USE_DISK_ACCESS,
    };
    int FS_RET_OK = 0;
    void set_mount_point() {
        mp_sd.mnt_point = disk_mount_pt.data();
    }

    SDLittleFSMount() {
        mp_sd.storage_dev = (void *) disk_name.data();
    };
};

typedef SDLittleFSMount SDMount;
#endif


template <typename MountClass>
class SDCardDisk 
{
public:
    SDCardDisk() {
        init();
    }
    ~SDCardDisk(){
        deinit();
    }

    MountClass mount;

private:
    int init();
    int deinit(); 


};

template <typename MountClass>
int SDCardDisk<MountClass>::init()
{
    int ret=-1;
    int ntry=0;
    while (ntry <5) { 
        ret = disk_access_ioctl(disk_name.data(), DISK_IOCTL_CTRL_INIT, NULL);
        if (ret == 0) {break;}
        LOG_WRN("could not access SD card - retry");
        ++ntry;
        k_sleep(K_MSEC(500));
    }
    if ( ret !=0 ) {
        LOG_ERR("SD card could not be initialized");
        return -EIO;
    }
    LOG_DBG("SD card disk access initialized successfully");

    mount.set_mount_point();

    int res = fs_mount(&mount.mp_sd);
    if (res != mount.FS_RET_OK) {
        LOG_ERR("Error mounting disk.\n");
        return -EIO;
    }
   return 0; 

}

template <typename MountClass>
int SDCardDisk<MountClass>::deinit()
{
    int res = fs_unmount(&mount.mp_sd);
    if (res != mount.FS_RET_OK) {
        LOG_ERR("Error unmounting disk");
        return -EIO;
    };
    if (disk_access_ioctl(disk_name.data(),
                          DISK_IOCTL_CTRL_DEINIT, NULL) != 0) {
        LOG_ERR("SD card can not be de-initialized");
        return -EIO;
    }
    return 0;
}

static auto sd_card = SDCardDisk<SDMount>();

void get_sd_full_path(char *path, const char* filename) {
    // TODO check
    strncpy(path, disk_mount_pt.data(), 128);
    int base = strlen(disk_mount_pt.data());
    path[base++] = '/';
    path[base] = 0;
    strcat(&path[base], filename);
    return;
}

void get_sd_timed_path(char *path,
                       const char* prefix,
                       struct tm* tm,
                       const char* suffix
                       ) {
    int lenp = strlen(prefix);
    char buf[128];
    strncpy(buf, prefix, lenp);
    get_time_string(tm, buf, lenp);

   strncpy(&buf[strlen(buf)], suffix, strlen(suffix)+1); // +1 for \0

   get_sd_full_path(path, buf); 
}

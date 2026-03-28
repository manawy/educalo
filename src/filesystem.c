#include "filesystem.h"
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/rtc.h>

#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>

LOG_MODULE_REGISTER(filesystem, LOG_LEVEL_INF);

#ifdef CONFIG_FAT_FILESYSTEM_ELM
#include <ff.h>

static FATFS fat_fs;

static struct fs_mount_t mp_sd = {
    .type = FS_FATFS,
    .fs_data = &fat_fs,
};

#define FS_RET_OK FR_OK
#endif


static const char *disk_mount_pt = DISK_MOUNT_POINT;

int init_sd_card() {
    do {
        static const char *disk_pdrv = DISK_DRIVE_NAME;
        
        int ret=-1;
        int try=0;
        while (try <5) { 
            ret = disk_access_ioctl(disk_pdrv, DISK_IOCTL_CTRL_INIT, NULL);
            if (ret == 0) {break;}
            LOG_WRN("could not access SD card - retry");
            ++try;
            k_sleep(K_MSEC(500));
        }
        if ( ret !=0 ) {
            LOG_ERR("SD card could not be initialized");
            break;
        }

        int64_t memory_size_mb;
        uint32_t block_count;
        uint32_t block_size;
        if (disk_access_ioctl(disk_pdrv,
                DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
            LOG_ERR("Unable to get sector count");
            break;
        }
        LOG_WRN("Block count %u", block_count);

        if (disk_access_ioctl(disk_pdrv,
                DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
            LOG_ERR("Unable to get sector size");
            break;
        }
        LOG_WRN("Sector size %u\n", block_size);

        memory_size_mb = (uint64_t)block_count * block_size;
        LOG_WRN("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));

        /*
        if (disk_access_ioctl(disk_pdrv,
                DISK_IOCTL_CTRL_DEINIT, NULL) != 0) {
            LOG_ERR("Storage deinit ERROR!");
            break;
        }*/

    } while (0);


    LOG_WRN("SD card disk access init");
    k_sleep(K_SECONDS(1));

    mp_sd.mnt_point = disk_mount_pt;

    int res = fs_mount(&mp_sd);
    if (res != FS_RET_OK) {
        LOG_ERR("Error mounting disk.\n");
        return -EIO;
    }
   return 0; 
}

int de_init_sd_card() {
    static const char *disk_pdrv = DISK_DRIVE_NAME;
    int res = fs_unmount(&mp_sd);
    if (res != FS_RET_OK) {
        LOG_ERR("Error unmounting disk");
        return -EIO;
    };
    if (disk_access_ioctl(disk_pdrv,
                          DISK_IOCTL_CTRL_DEINIT, NULL) != 0) {
        LOG_ERR("SD card can not be de-initialized");
        return -EIO;
    }
    return 0;
}

void get_sd_full_path(char *path, const char* filename) {
    // TODO check
    strncpy(path, disk_mount_pt, 128);
    int base = strlen(disk_mount_pt);
    path[base++] = '/';
    path[base] = 0;
    strcat(&path[base], filename);
    return;
}


const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

void get_sd_timed_path(char *path, const char* prefix) {
    struct rtc_time tm;
    int ret = rtc_get_time(rtc, &tm);
    if (ret <0) {
        LOG_ERR("Cannot get RTC time !");
    }


    int lenp = strlen(prefix);
    char buf[128];
    strncpy(buf, prefix, lenp);

    snprintfcb(&buf[lenp], 128-lenp, 
               "%02d%02d%02d_%02d%02d%02d",
                tm.tm_year-100,
                tm.tm_mon+1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec
               ); 
   get_sd_full_path(path, buf); 
}

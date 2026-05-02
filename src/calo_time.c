// SPDX-FileCopyrightText: Copyright Fabien Georget
// SPDX-License-Identifier: Apache-2.0 

#include "calo_time.h"

#ifdef CONFIG_POSIX_API
#define _POSIX_C_SOURCE 200112L
#include <time.h>
#endif

#include <zephyr/sys/clock.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>



LOG_MODULE_REGISTER(time, CONFIG_LOG_DEFAULT_LEVEL);

#ifdef CONFIG_RTC_EMUL
#include <native_rtc.h>
#endif

// The real time clock device
static const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

#ifdef CONFIG_RTC_EMUL
// Initialize the emulated rtc device for native_sim
void init_time_emul()
{
        struct timespec ts ={.tv_sec=0,.tv_nsec=0};
        uint32_t nsec = 0;
        native_rtc_gettime(RTC_CLOCK_PSEUDOHOSTREALTIME, &nsec, (uint64_t*) &ts.tv_sec);
        struct rtc_time tm;
        gmtime_r(&ts.tv_sec, rtc_time_to_tm(&tm));
        rtc_set_time(rtc, &tm);
}
#endif

#ifdef CONFIG_RTC
// Initialize the sysclock by synchronisation with the external rtc
static int set_time_from_external_rtc() {
    struct rtc_time tm;
    struct timespec tp;

    int ret = rtc_get_time(rtc, &tm);
    if (ret <0) {
        LOG_ERR("Cannot get RTC time ! return code : %d", ret);
        return ret;
    }
    tp.tv_sec = timeutil_timegm(rtc_time_to_tm(&tm));
    if (tp.tv_sec == -1) {
        LOG_ERR("Cannot calculated seconds since EPOCH");
        return ret;
    }
    tp.tv_nsec = 0;

    sys_clock_settime(SYS_CLOCK_REALTIME, &tp);
    return 0;
}
#endif

int get_time(struct tm *tm)
{
    struct timespec tp;
    sys_clock_gettime(SYS_CLOCK_REALTIME, &tp);
    gmtime_r(&tp.tv_sec, tm);
    return 0;
}

int get_time_string(struct tm *tm, char *buf, int offset)
{
    snprintfcb(&buf[offset], 15,
               "%02d%02d%02d_%02d%02d%02d",
                tm->tm_year-100,
                tm->tm_mon+1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec
               );
    return 0;
}

int init_time() 
{
    #ifdef CONFIG_RTC_EMUL
    init_time_emul();
    #endif

    #ifdef CONFIG_RTC
    set_time_from_external_rtc();
    #endif

    return 0;
}


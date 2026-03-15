#include "sensor_thread.h"

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

// --- Logging ---
LOG_MODULE_REGISTER(sensor_thread, LOG_LEVEL_INF);

// --- Zbus ------
ZBUS_SUBSCRIBER_DEFINE(sensor_thread_sub, 4);
ZBUS_OBS_DECLARE(processing_thread_sub);

ZBUS_CHAN_DEFINE(sensor_data_chan,
                 struct sensor_data_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(processing_thread_sub),
                 ZBUS_MSG_INIT(0)
                 );

// --- Device tree -----------
static const struct adc_dt_spec fluxsensor =
    ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

// --- Sensors -----

static int adc_init(struct adc_sequence* sequence) {
    adc_sequence_init_dt(&fluxsensor, sequence);
    if (!adc_is_ready_dt(&fluxsensor)) {
        LOG_ERR("ADC controller device not ready");
        return -1;
    }
    int err = adc_channel_setup_dt(&fluxsensor);
    if (err < 0) {
        LOG_ERR("Could not set up ADC");
        return err;
    }
    LOG_INF("ADC: initialized");
    return 0;
}

static bool adc_measure_heat(struct adc_sequence* sequence, int32_t* value) {
    int ret = adc_read_dt(&fluxsensor, sequence);
    if (ret < 0) {
        LOG_ERR("Could not read ADC %d\n", ret);
        return false;
    }
    *value = (int32_t)(*((int16_t*) sequence->buffer));
    adc_raw_to_microvolts_dt(&fluxsensor, value);
    return true;
}

// --- Thread definition

void sensor_thread() {
    const struct zbus_channel* chan;
    struct sensor_data_msg sdata = {.uv = 0, .ok=false};

    uint16_t buffer;
    struct adc_sequence sequence;
    sequence.buffer = &buffer;
    sequence.buffer_size = sizeof(buffer);

    adc_init(&sequence);

    while(1) {
        zbus_sub_wait(&sensor_thread_sub, &chan, K_FOREVER);

        bool ok = adc_measure_heat(&sequence, &sdata.uv);
        sdata.ok = ok;

        zbus_chan_pub(&sensor_data_chan, &sdata, K_MSEC(100));
    }
}

K_THREAD_DEFINE(sensor_thread_id,
                1024,
                sensor_thread,
                NULL, NULL, NULL,
                3, 0,
                0);



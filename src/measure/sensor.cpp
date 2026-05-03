/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include "measure/sensor.hpp"
#include "zephyr/devicetree.h"
#include "zbus_channels.h"
#include "zephyr/zbus/zbus.h"

#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

#include <zephyr/drivers/sensor.h>


// --- Logging ---
LOG_MODULE_REGISTER(sensor_thread, LOG_LEVEL_INF);

// --- Device tree -----------
#ifdef CONFIG_DT_HAS_TI_ADS1115_ENABLED
static const struct adc_dt_spec fluxsensor =
    ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);


// --- Sensors -----

static int adc_init(struct adc_sequence* sequence) {

    if (!adc_is_ready_dt(&fluxsensor)) {
        LOG_ERR("ADC controller device not ready");
        return -1;
    }
    adc_sequence_init_dt(&fluxsensor, sequence);
    int err = adc_channel_setup_dt(&fluxsensor);
    if (err < 0) {
        LOG_ERR("Could not set up ADC");
        return err;
    }
    LOG_INF("ADC: initialized");
    return 0;
}
#endif


#ifdef CONFIG_DT_HAS_TI_ADS1115_ENABLED
struct k_poll_signal signal = K_POLL_SIGNAL_INITIALIZER(signal);
struct k_poll_event event = K_POLL_EVENT_STATIC_INITIALIZER(
                                K_POLL_TYPE_SIGNAL,
                                K_POLL_MODE_NOTIFY_ONLY,
                                &signal, 0);

static bool adc_measure_heat(struct adc_sequence* sequence, int32_t* value) {
    
    LOG_DBG("start reading");
    int ret = adc_read_async_dt(&fluxsensor, sequence, &signal);
    if (ret < 0) {
        LOG_ERR("Could not read ADC %d\n", ret);
        return false;
    }
    ret = k_poll(&event, 1, K_MSEC(100));
    if (event.state == K_POLL_STATE_SIGNALED) {
        event.state = K_POLL_STATE_NOT_READY;
        *value = (int32_t)(*((int16_t*) sequence->buffer));
        adc_raw_to_microvolts_dt(&fluxsensor, value);
        return true;
    } else {
        LOG_WRN("Timeout on I2C ADC reading !");
        return false;
    }
}
#endif



// --- Thread definition

#ifdef CONFIG_DT_HAS_TI_ADS1115_ENABLED
static void get_and_publish() {
    struct sensor_data_msg sdata;

        //bool ok = false;
        int32_t value;
        bool ok = adc_measure_heat(&sequence, &value);
        //LOG_PRINTK("data: %d", value);
        //LOG_PRINTK("data ok ?: %d\n", sdata.ok);
        sdata.ok = ok;
        sdata.uv.val1 = value;

    zbus_chan_pub(&sensor_data_chan, &sdata, K_MSEC(100));
}
#endif

class FluxSensor
{
public:
    FluxSensor() = default;

    int init() ;

    int get_and_publish(); 

    void set_attr(const enum sensor_attribute attr, const struct sensor_value& val) {
        sensor_attr_set(fluxsensor, SENSOR_CHAN_VOLTAGE, attr, &val);
    }

    struct sensor_value get_attr(const enum sensor_attribute attr) {
        struct sensor_value val;
        sensor_attr_get(fluxsensor, SENSOR_CHAN_VOLTAGE, attr, &val);
        return val;
    }

    struct sensor_value get_fsr() {
        return get_attr(SENSOR_ATTR_GAIN);
    }

private:
    const struct device *fluxsensor = DEVICE_DT_GET(DT_ALIAS(fluxsensor));
    struct sensor_value m_val;
    struct sensor_data_msg m_pub;
};

int FluxSensor::init() 
{
    if (!device_is_ready(fluxsensor)) {
        LOG_ERR_DEVICE_NOT_READY(fluxsensor);
        return -EIO;
    }
    struct sensor_value val;
    sensor_value_from_float(&val, 1.024);
    sensor_attr_set(fluxsensor, SENSOR_CHAN_VOLTAGE, SENSOR_ATTR_GAIN, &val);
    return 0;
}

int FluxSensor::get_and_publish() 
{
    int rc = sensor_sample_fetch(fluxsensor);
    if (rc != 0) {
        m_pub.ok = false;
        LOG_ERR("Could not get reading from flux sensor");
        zbus_chan_pub(&sensor_data_chan, &m_pub, K_MSEC(50));
        return -1;
    }
    sensor_channel_get(fluxsensor, SENSOR_CHAN_VOLTAGE, &m_val);
    m_pub.uv = m_val;
    m_pub.ok = true;
    return zbus_chan_pub(&sensor_data_chan, &m_pub, K_MSEC(50));

}

FluxSensor sensor;

struct sensor_value get_sensor_current_fsr() {
    return sensor.get_fsr();
}

void sensor_thread() {

    const struct zbus_channel* chan;
 
    #ifdef CONFIG_DT_HAS_TI_ADS1115_ENABLED
    uint16_t buffer;
    struct adc_sequence sequence;
    sequence.buffer = &buffer;
    sequence.buffer_size = sizeof(buffer);

    if (adc_init(&sequence) < 0) {
        k_sleep(K_SECONDS(1));
        adc_init(&sequence);
    }
    #elif defined CONFIG_FLUXSENSOR
    FluxSensor sensor;
    sensor.init();
    #endif

    while(1) {
        zbus_sub_wait(&sensor_thread_sub, &chan, K_FOREVER);

        if (&sensor_attr_chan == chan) {
            #ifdef CONFIG_FLUXSENSOR
            struct sensor_attr_msg msg;
            zbus_chan_read(&sensor_data_chan, &msg, K_MSEC(10));
            sensor.set_attr(msg.attr, msg.val);
            #endif
            continue;
        } else if (&start_trigger_chan == chan) {
            #ifdef CONFIG_FLUXSENSOR
            sensor.get_and_publish();
            #else
            get_and_publish();
            #endif
            continue;
        } else {
            LOG_ERR("Invalid ZBUS channel !");
        }
    }
}

K_THREAD_DEFINE(sensor_thread_id,
                2048,
                sensor_thread,
                NULL, NULL, NULL,
                CONFIG_SENSOR_THREAD_PRIORITY, 0,
                0);



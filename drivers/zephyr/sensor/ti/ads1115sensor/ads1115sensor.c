/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/syscalls/kernel.h>
#include <zephyr/sys/__assert.h>
#include <stdint.h>
#define DT_DRV_COMPAT ti_ads1115sensor

#include "ads1115sensor.h"

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ADS1115sensor, CONFIG_SENSOR_LOG_LEVEL);

int ads1115_read(const struct device *dev){
    return 0;
}

int ads1115_write_config(const struct device *dev){

    const struct ads1115sensor_config *cfg = dev->config;
    const struct ads1115sensor_data *data = dev->data;

    int ret = i2c_write_dt(&cfg->i2c, data->config_buf, 3);
    if (ret < 0) {
        LOG_ERR("Error writing config !");
        return -EIO;
    }

    return 0;

}

int ads1115_read_value(const struct device *dev) {

    const struct ads1115sensor_config *cfg = dev->config;
    struct ads1115sensor_data *data = dev->data;
    int ret;

    uint8_t buf[2];
    ret = i2c_burst_read_dt(&cfg->i2c, ADS1115_APOINTER_CONVERSION, buf, 2);
    if (ret < 0) {
        LOG_ERR("Failed reading value from ADS1115 sensor");
        return ret;
    }
    data->reg_val = (buf[0] << 8) | buf[1];
    return ret;
}

static int ads1115sensor_sample_fetch(const struct device *dev, enum sensor_channel chan){
    struct ads1115sensor_data *dev_data = dev->data;
    int ret;

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || 
                    chan == SENSOR_CHAN_VOLTAGE)

    // Clear sensor values
    dev_data->reg_val = 0;

    ret = ads1115_write_config(dev);
    if (ret < 0) {
        return ret;
    }
    k_sleep(K_MSEC(10)); // according datasheet
    ret = ads1115_read_value(dev);
    if (ret < 0) {

    }


    return 0;
}

static int ads1115sensor_channel_get(
    const struct device *dev,
    enum sensor_channel chan,
    struct sensor_value *val) {

    int64_t tmp;

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || 
                    chan == SENSOR_CHAN_VOLTAGE)

    struct ads1115sensor_data *dev_data = dev->data;

    tmp = (int64_t)dev_data->reg_val * 512000;
    val->val1 = (int32_t)tmp /(32768);
    val->val2 = (tmp % 32768);

    return 0;
}

static DEVICE_API(sensor, ads1115sensor_api_funcs) = {
    .sample_fetch = ads1115sensor_sample_fetch,
    .channel_get = ads1115sensor_channel_get,
};

int ads1115sensor_init(const struct device *dev) {

    struct ads1115sensor_data *dev_data = dev->data;
    const struct ads1115sensor_config *dev_cfg = dev->config;

    if (!device_is_ready(dev_cfg->i2c.bus)) {
        LOG_ERR_DEVICE_NOT_READY(dev_cfg->i2c.bus);
        return -EINVAL;
    }

    dev_data->config_buf[0] = ADS1115_APOINTER_CONFIG;
    dev_data->config_buf[1] = ADS1115_CONFIG_MSB_OS_TRIGGER |
                              ADS1115_CONFIG_MSB_MUX_03 |
                              ADS1115_CONFIG_MSB_PGA_FSR_0_512 |
                              ADS1115_CONFIG_MSB_MODE_SINGLE;
    dev_data->config_buf[2] = ADS1115_CONFIG_LSB_DR_128SPS |
                              ADS1115_CONFIG_LSB_COMPMODE_TRADITIONAL |
                              ADS1115_CONFIG_LSB_COMPPOL_LOW |
                              ADS1115_CONFIG_LSB_COMPLATCH_FALSE |
                              ADS1115_CONFIG_LSB_COMPALERT_DISABLE;

    return 0;
}

#define ADS1115SENSOR_DEFINE(inst)                                              \
    static struct ads1115sensor_data ads1115sensor_data_##inst;                 \
                                                                                \
    static const struct ads1115sensor_config ads1115sensor_config_##inst = {    \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                                      \
        };                                                                      \
                                                                                \
    SENSOR_DEVICE_DT_INST_DEFINE(inst,                                          \
                                 ads1115sensor_init,                            \
                                 NULL,                                          \
                                 &ads1115sensor_data_##inst,                    \
                                 &ads1115sensor_config_##inst,                  \
                                 POST_KERNEL,                                   \
                                 CONFIG_SENSOR_INIT_PRIORITY,                   \
                                 &ads1115sensor_api_funcs                       \
                             );

DT_INST_FOREACH_STATUS_OKAY(ADS1115SENSOR_DEFINE)


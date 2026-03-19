#include <leds_interface.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(leds_interface, LOG_LEVEL_WRN);

static const struct gpio_dt_spec led_busy = GPIO_DT_SPEC_GET(DT_ALIAS(ledbusy), gpios);
static const struct gpio_dt_spec led_ok = GPIO_DT_SPEC_GET(DT_ALIAS(ledok), gpios);

ZBUS_CHAN_DECLARE(start_trigger_chan)   ;

int ledbusy_init() {
    if (!gpio_is_ready_dt(&led_busy)) {
        LOG_ERR("led_busy not ready");
        return -1;
    }
    return 0;
}

int ledon_init() {
    if (!gpio_is_ready_dt(&led_ok)) {
        LOG_ERR("ledok not ready");
        return -1;
    }
    return 0;
}

void ledon_start() {
    int ret = gpio_pin_configure_dt(&led_ok, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot set led_busy");
    }
    return;
}

void ledbusy_set_busy() {
    int ret = gpio_pin_configure_dt(&led_busy, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot set led_busy");
    }
    return;
}

void ledbusy_set_free() {
    int ret = gpio_pin_configure_dt(&led_busy, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Cannot set led_busy");
    }
    return;
}

void listener_ledbusy_set(const struct zbus_channel *chan){
    if (&start_trigger_chan == chan) {
        ledbusy_set_busy();
    } else {
        ledbusy_set_free();
    }
    return;
}


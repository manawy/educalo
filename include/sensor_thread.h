#ifndef CALOBUS_SENSORTHREAD_H
#define CALOBUS_SENSORTHREAD_H

#include <stdint.h>
#include <stdbool.h>

// The zbus message containing the raw data
struct sensor_data_msg {
    int32_t uv;
    bool ok;
};

#endif // CALOBUS_SENSORTHREAD_H

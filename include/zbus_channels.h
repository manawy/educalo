#include <zephyr/zbus/zbus.h>

ZBUS_OBS_DECLARE(sensor_thread_sub);
ZBUS_OBS_DECLARE(processing_thread_sub);
ZBUS_OBS_DECLARE(datalogger_thread_sub);

ZBUS_CHAN_DECLARE(start_measure_chan);
ZBUS_CHAN_DECLARE(end_measure_chan);

ZBUS_CHAN_DECLARE(start_trigger_chan);
ZBUS_CHAN_DECLARE(sensor_data_chan);
ZBUS_CHAN_DECLARE(processing_thread_chan);
ZBUS_CHAN_DECLARE(sensor_attr_chan);
ZBUS_CHAN_DECLARE(end_onebeat_chan) ;

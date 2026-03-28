cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers
)


find_package(Zephyr)
project(CaloBus)


#add_subdirectory(drivers)

target_sources(app PRIVATE 
    src/main.c
    src/app_info.c
    src/toggle_measurement.c
    src/sensor_thread.c
    src/processing_thread.c
    src/datalogger_thread.c
    src/leds_interface.c
    src/filesystem.c
)
target_include_directories(app PRIVATE include)

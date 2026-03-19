cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(CaloBus)

target_sources(app PRIVATE 
    src/main.c
    src/app_info.c
    src/sensor_thread.c
    src/processing_thread.c
    src/datalogger_thread.c
    src/leds_interface.c
)
target_include_directories(app PRIVATE include)

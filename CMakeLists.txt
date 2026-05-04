cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers
)


find_package(Zephyr)
project(CaloBus)


#add_subdirectory(rivers)

set (APP_SOURCES

    src/main.cpp

    src/calo_time.c
    src/filesystem.cpp
    src/health_check.cpp
    src/zbus_channels.c

    src/measure/datalogger.cpp
    src/measure/processor.cpp
    src/measure/sensor.cpp
    src/measure/toggle.cpp

    src/interface/init.c
    src/interface/leds.c

)

macro(add_source_if_def config_option file)
    if(DEFINED ${config_option})
        list(APPEND APP_SOURCES ${file})
    endif()
endmacro()

add_source_if_def(CONFIG_SHELL src/interface/shell.c)
add_source_if_def(CONFIG_INPUT src/interface/input.c)

target_sources(app PRIVATE ${APP_SOURCES})

target_include_directories(app PRIVATE include)

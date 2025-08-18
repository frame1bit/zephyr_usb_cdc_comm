# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(usb_comm)

include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

FILE(GLOB app_sources 
    src/*.c
    src/comm/*.c
    src/user_app/*.c
)

if(CONFIG_APDS9960)
    target_sources(app PRIVATE src/sensor/apds9960.c)
    include_directories(app PRIVATE src/sensor)
endif()

target_sources(app PRIVATE ${app_sources})

include_directories(app PRIVATE
    src/comm
    src/user_app
)
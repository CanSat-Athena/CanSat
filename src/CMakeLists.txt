cmake_minimum_required(VERSION 3.13)

add_executable(cansat
        cansat.cpp
        globals.cpp
        dht20.cpp
        anemometer.cpp
        bme680.cpp
        imu.cpp
        lightSensor.cpp
        gps.cpp
        watchdog.cpp
        filesystem.cpp
        littlefs.cpp
        dataHandler.cpp
        bme68x/bme68x.cpp
        bme68x/BoschBME68XApi/bme68x.c
        littlefs/lfs.c
        littlefs/lfs_util.c
        pico-icm20948/src/pico-icm20948.c
        exceptionHandlers.h
        i2c.h
        ../Config/FreeRTOSConfig.cpp
        )

target_include_directories(cansat PRIVATE
        ${CMAKE_SOURCE_DIR}/
        ${CMAKE_SOURCE_DIR}/src
        Config/
        lib/FreeRTOS-Kernel/include
        lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0
        ${PICO_SDK_PATH}/**
)

add_compile_definitions(
        # FREE_RTOS_KERNEL_SMP=1
        # portSUPPORT_SMP=1
        PICO_FLASH_SIZE_BYTES=16777216
        LFS_THREADSAFE
        CMAKE_BUILD_TYPE=Debug
        LWGPS_IGNORE_USER_OPTS
)

add_subdirectory(pico-icm20948)

target_link_libraries(cansat PUBLIC
        freertos_config
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_gpio
        hardware_timer
        hardware_adc
        hardware_uart
        hardware_watchdog
        hardware_clocks
        hardware_exception
        hardware_irq
        hardware_flash
        pico_multicore
        FreeRTOS-Kernel-Heap4
        lwgps
        pico-icm20948
        )

pico_enable_stdio_usb(cansat 1)
pico_enable_stdio_uart(cansat 1)

pico_add_extra_outputs(cansat)
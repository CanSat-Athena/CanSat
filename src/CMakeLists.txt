cmake_minimum_required(VERSION 3.13)

add_executable(cansat
        cansat.cpp
        dht20.cpp
        bme680.cpp
        lightSensor.cpp
        bme68x/bme68x.cpp
        bme68x/BoschBME68XApi/bme68x.c
        exceptionHandlers.h
        i2c.h
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
        FREE_RTOS_KERNEL_SMP=1
        portSUPPORT_SMP=1
)

target_link_libraries(cansat PUBLIC
        freertos_config
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_timer
        hardware_adc
        hardware_watchdog
        hardware_clocks
        hardware_exception
        pico_multicore
        FreeRTOS-Kernel-Heap4
        )

pico_enable_stdio_usb(cansat 1)
pico_enable_stdio_uart(cansat 1)

pico_add_extra_outputs(cansat)

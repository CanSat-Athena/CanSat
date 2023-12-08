cmake_minimum_required(VERSION 3.13)

add_executable(cansat
        cansat.cpp
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
        pico_multicore
        hardware_exception
        FreeRTOS-Kernel-Heap4
        )

pico_enable_stdio_usb(cansat 1)
pico_enable_stdio_uart(cansat 1)

pico_add_extra_outputs(cansat)

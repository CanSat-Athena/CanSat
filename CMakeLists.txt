cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico)

#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Import those libraries
include(pico_sdk_import.cmake)

# Define project
project(cansat C CXX ASM)
SET(PROJECT_NAME cansat)
#enable_language( C CXX ASM )

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Set any variables required for importing libraries
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(lib)
#include <pico/stdlib.h>
// #include "lwgps/lwgps.hpp"

// PINS & PORTS
#define I2C_PORT i2c0
#define GPS_UART uart1
#define GPS_TX_PIN 8
#define GPS_RX_PIN 9
#define ANEMOMETER_PIN 6

// I2C Addresses
#define DHT20_ADDRESS  0x38
#define BME680_ADDRESS (uint8_t)0x77
#define IMU_ADDRESS (uint8_t)0x69

// Other config
#define DHT20_PROCESSING_TIMEOUT_MS 500
#define I2C_PER_CHAR_TIMEOUT_US 600

// Filesystem
#define FS_SIZE (1024 * 1024) // 1MB
// #define NUKE_FS_ON_NEXT_BOOT

// Watchdog
#define WATCHDOG_TIME 10000
#define WATCHDOG_TASK_TIME 500          // Must be significantly less than WATCHDOG_TIME

// Let's make it idiot-proof
#if WATCHDOG_TASK_TIME > WATCHDOG_TIME
#warning WATCHDOG_TASK_TIME is less than WATCHDOG_TIME, watchdog will cause a reboot
#endif

// Sensor read frequencies
#define DHT20_READ_FREQ 1
#define BME680_READ_FREQ 1
#define IMU_READ_FREQ 20        // Also need to modify pico-icm20948.cpp
#define LIGHT_READ_FREQ 1
#define ANEMOMETER_READ_FREQ 1

// Sensor delay times (ms)
#define DHT20_READ_TIME (uint32_t)(1000.0f / DHT20_READ_FREQ)
#define BME680_READ_TIME (uint32_t)(1000.0f / BME680_READ_FREQ)
#define IMU_READ_TIME (uint32_t)(1000.0f / IMU_READ_FREQ)
#define LIGHT_READ_TIME (uint32_t)(1000.0f / LIGHT_READ_FREQ)
#define ANEMOMETER_READ_TIME (uint32_t)(1000.0f / ANEMOMETER_READ_FREQ)

// Use when debugging
#define DEBUG_MODE

#define VERBOSE_PRINT true
#define EXTRA_VERBOSE_BME680 false
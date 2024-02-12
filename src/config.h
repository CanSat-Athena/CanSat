#include <pico/stdlib.h>
// #include "lwgps/lwgps.hpp"

// PINS & PORTS
#define I2C_PORT i2c0
#define GPS_UART uart1
#define GPS_TX_PIN 8
#define GPS_RX_PIN 9

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

// Sensor read frequencies
#define DHT20_READ_FREQ 1
#define BME680_READ_FREQ 0.5
#define LIGHT_READ_FREQ 5

// Sensor delay times (ms)
#define DHT20_READ_TIME (uint32_t)(1000.0f / DHT20_READ_FREQ)
#define BME680_READ_TIME (uint32_t)(1000.0f / BME680_READ_FREQ)
#define LIGHT_READ_TIME (uint32_t)(1000.0f / LIGHT_READ_FREQ)

// Use when debugging
#define DEBUG_MODE

#define VERBOSE_PRINT true
#define EXTRA_VERBOSE_BME680 false
#include <pico/stdlib.h>

// PINS & PORTS
#define I2C_PORT i2c0

// I2C Addresses
#define DHT20_ADDRESS  0x38
#define BME680_ADDRESS (uint8_t)0x77

// Other config
#define DHT20_PROCESSING_TIMEOUT_MS 500
#define I2C_PER_CHAR_TIMEOUT_US 600

// Filesystem
#define FS_SIZE (256 * 1024)
// #define NUKE_FS_ON_NEXT_BOOT

// Sensor read frequencies
#define DHT20_READ_FREQ 1
#define BME680_READ_FREQ 0.5
#define LIGHT_READ_FREQ 10

// Sensor delay times (ms)
#define DHT20_READ_TIME (uint32_t)(1000.0f / DHT20_READ_FREQ)
#define BME680_READ_TIME (uint32_t)(1000.0f / BME680_READ_FREQ)
#define LIGHT_READ_TIME (uint32_t)(1000.0f / LIGHT_READ_FREQ)

// Use when debugging
#define DEBUG_MODE

#define VERBOSE_PRINT true
#define EXTRA_VERBOSE_BME680 false
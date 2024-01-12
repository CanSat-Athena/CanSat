// PINS & PORTS
#define I2C_PORT i2c0

// I2C Addresses
#define DHT20_ADDRESS  0x38
#define BME680_ADDRESS (uint8_t)0x77

// Other config
#define DHT20_PROCESSING_TIMEOUT_MS 500
#define I2C_PER_CHAR_TIMEOUT_US 200

// Filesystem
#define FS_SIZE (256 * 1024)
// #define NUKE_FS_ON_NEXT_BOOT

// Use when debugging
#define VERBOSE_PRINT true

#define EXTRA_VERBOSE_BME680 false
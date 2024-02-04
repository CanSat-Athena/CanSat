#pragma once
#include <pico/stdlib.h>
#include <hardware/i2c.h>
// #include <FreeRTOS.h>
// #include <semphr.h>

#include "config.h"

class I2C {
public:
    // SemaphoreHandle_t i2cMutex;
    /// @brief Initialise I2C
    /// @param sdaPin The GPIO pin SDA is connected to
    /// @param sclPin The GPIO pin SCL is connected to
    static void init(const int sdaPin = PICO_DEFAULT_I2C_SDA_PIN, const int sclPin = PICO_DEFAULT_I2C_SCL_PIN) {
        // Setup I2C properly
        gpio_init(sdaPin);
        gpio_init(sclPin);
        gpio_set_function(sdaPin, GPIO_FUNC_I2C);
        gpio_set_function(sclPin, GPIO_FUNC_I2C);
        // Don't forget the pull ups! | Or use external ones
        gpio_pull_up(sdaPin);
        gpio_pull_up(sclPin);

        // Init I2C
        i2c_init(I2C_PORT, 100 * 1000); // 100kHz
    }

    /// @brief Reads I2C register of device
    /// @param address Address of I2C device
    /// @param reg Register of I2C device to read
    /// @param buf The buffer to read to
    /// @param len Number of bytes to read
    /// @return Number of bytes read, or PICO_ERROR_GENERIC if address not acknowledged or no device present.
    static int readRegister(const uint8_t address, const uint8_t reg, uint8_t* buf, const size_t len) {
        uint8_t res = i2c_write_timeout_per_char_us(I2C_PORT, address, &reg, 1, true, I2C_PER_CHAR_TIMEOUT_US);
        return i2c_read_timeout_per_char_us(I2C_PORT, address, buf, len, false, I2C_PER_CHAR_TIMEOUT_US);
    }

    /// @brief Writes to I2C register of device
    /// @param address Address of I2C device
    /// @param reg Register of I2C device to write to
    /// @param buf The buffer to write
    /// @param len Number of bytes to write
    /// @return Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged or no device present.
    static int writeRegister(const uint8_t address, const uint8_t reg, const uint8_t* buf, const size_t len) {
        i2c_write_timeout_per_char_us(I2C_PORT, address, &reg, 1, true, I2C_PER_CHAR_TIMEOUT_US);
        return i2c_write_timeout_per_char_us(I2C_PORT, address, buf, len, false, I2C_PER_CHAR_TIMEOUT_US);
    }
};
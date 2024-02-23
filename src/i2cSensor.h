#pragma once
#include <pico/stdlib.h>
#include <stdio.h>

#include "sensor.h"
#include "config.h"
#include "i2c.h"

class I2CSensor : public Sensor {
protected:
    uint8_t i2cAddress;

    /// @brief Read status byte of sensor (if possible with sensor)
    /// @return The status byte read
    uint8_t readStatus() {
        uint8_t read;

        // Return 0xFF if error
        I2C::take();
        if (!i2c_read_timeout_per_char_us(I2C_PORT, i2cAddress, &read, 1, false, I2C_PER_CHAR_TIMEOUT_US)) {
            I2C::give();
            return 0xFF;
        }
        I2C::give();
        return read;
    }

    /// @brief Reads I2C register of device
    /// @param reg Register of I2C device to read
    /// @param buf The buffer to read to
    /// @param len Number of bytes to read
    /// @return Number of bytes read, or PICO_ERROR_GENERIC if address not acknowledged or no device present.
    int readRegister(const uint8_t reg, uint8_t* buf, const size_t len) {
        uint8_t res = i2c_write_timeout_per_char_us(I2C_PORT, i2cAddress, &reg, 1, true, I2C_PER_CHAR_TIMEOUT_US);
        return i2c_read_timeout_per_char_us(I2C_PORT, i2cAddress, buf, len, false, I2C_PER_CHAR_TIMEOUT_US);
    }

    /// @brief Writes to I2C register of device
    /// @param reg Register of I2C device to write to
    /// @param buf The buffer to write
    /// @param len Number of bytes to write
    /// @return Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged or no device present.
    int writeRegister(const uint8_t reg, const uint8_t* buf, const size_t len) {
        i2c_write_timeout_per_char_us(I2C_PORT, i2cAddress, &reg, 1, true, I2C_PER_CHAR_TIMEOUT_US);
        return i2c_write_timeout_per_char_us(I2C_PORT, i2cAddress, buf, len, false, I2C_PER_CHAR_TIMEOUT_US);
    }

public:
    I2CSensor(const uint8_t i2cAddress) {
        this->i2cAddress = i2cAddress;
    }
};
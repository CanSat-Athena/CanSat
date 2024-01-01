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
        if (!i2c_read_timeout_per_char_us(I2C_PORT, i2cAddress, &read, 1, false, I2C_PER_CHAR_TIMEOUT_US)) {
            return 0xFF;
        }
        return read;
    }

public:
    I2CSensor(const uint8_t i2cAddress) {
        this->i2cAddress = i2cAddress;
    }
};
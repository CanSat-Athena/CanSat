#include "pico/stdlib.h"
#include <stdio.h>

#include "sensor.h"
#include "pins.h"
#include "i2c.h"

class I2CSensor : public Sensor {
protected:
    uint8_t i2cAddress;

    /// @brief Read register of sensor, wrapper around I2C::readRegister
    int readRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false) {
        if (!initialised && !ignoreInit) return -1;
        return I2C::readRegister(i2cAddress, reg, buf, len);
    }

    /// @brief Write to register of sensor, wrapper around I2C::writeRegister
    int writeRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false) {
        if (!initialised && !ignoreInit) return -1;
        return I2C::writeRegister(i2cAddress, reg, buf, len);
    }

    /// @brief Read status byte of sensor (if possible with sensor)
    /// @return The status byte read
    uint8_t readStatus() {
        uint8_t read;

        // Return 0xFF if error
        if (!i2c_read_blocking(I2C_PORT, i2cAddress, &read, 1, false)) {
            return 0xFF;
        }
        return read;
    }

public:
    I2CSensor(uint8_t i2cAddress) {
        this->i2cAddress = i2cAddress;
    }
};
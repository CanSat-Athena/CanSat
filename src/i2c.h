#pragma once
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pins.h"

class I2C {
public:
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
    
    // Read register of an i2c device using its address, register to read, length of read and buffer
    static int readRegister(const uint8_t address, const uint8_t reg, uint8_t *buf, const size_t len)  {
        i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
        return i2c_read_blocking(I2C_PORT, address, buf, len, false);
    }
};
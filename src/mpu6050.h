#pragma once
#include "pico/stdlib.h"
#include <stdio.h>

#include "pins.h"
#include "i2c.h"

#define WHOAMI_REG 0x75

class MPU6050 {
private:
    bool initialised = false;

protected:
    int readRegister(const uint8_t reg, uint8_t *buf, const size_t len, const bool ignoreInit = false);

public:
    MPU6050(bool initialise = true);

    // Returns true if connection to sensor succeeded
    bool init(const uint attempts = 3);

    /// @brief Checks if MPU6050 is initialised
    /// @return true if MPU6050 has been initialised
    bool isInitialised() {
        return initialised;
    }
};
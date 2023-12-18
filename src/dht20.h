#pragma once
#include "pico/stdlib.h"
#include <stdio.h>

#include "pins.h"
#include "i2c.h"

#define DHT20_STATUSWORD_REG 0x71
#define DHT20_INITIALISE_REG 0xbe

class DHT20 {
private:
    bool initialised = false;

protected:
    int readRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false);
    int writeRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false);

public:
    DHT20(bool initialise = true);

    bool init(const uint attempts = 3);

    /// @brief Checks if DHT20 is initialised
    /// @return true if DHT20 has been initialised
    bool isInitialised() {
        return initialised;
    }
};
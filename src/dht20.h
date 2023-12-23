#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include <stdio.h>

#include "pins.h"
#include "i2c.h"

#define DHT20_STATUSWORD_REG    0x71
#define DHT20_CALIBRATE_CMD     0xE1
#define DHT20_READ_CMD          0xAC
#define DHT20_SOFTRESET_CMD     0xAC
#define DHT20_STATUS_BUSY       0x80
#define DHT20_STATUS_CALIBRATED 0x08

class DHT20 {
private:
    bool initialised = false;

protected:
    int readRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false);
    int writeRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false);

    uint8_t readStatus();

    bool waitForProcessing(const bool useRTOSDelay = true);

public:
    float temperature = 0;
    float humidity = 0;
    uint64_t lastUpdated;

    DHT20(bool initialise = true);

    bool init(const uint attempts = 3);
    bool updateData();

    /// @brief Checks if DHT20 is initialised
    /// @return true if DHT20 has been initialised
    bool isInitialised() {
        return initialised;
    }
};
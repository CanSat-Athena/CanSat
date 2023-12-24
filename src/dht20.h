#pragma once
#include "pico/stdlib.h"
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "i2cSensor.h"

#define DHT20_STATUSWORD_REG    0x71
#define DHT20_CALIBRATE_CMD     0xE1
#define DHT20_READ_CMD          0xAC
#define DHT20_SOFTRESET_CMD     0xAC
#define DHT20_STATUS_BUSY       0x80
#define DHT20_STATUS_CALIBRATED 0x08

class DHT20 : public I2CSensor {
protected:
    bool waitForProcessing(const bool useRTOSDelay = true);

public:
    float temperature = 0;
    float humidity = 0;
    uint64_t lastUpdated;

    DHT20(bool initialise = true);

    bool init(const uint attempts = 3);
    bool updateData();
};
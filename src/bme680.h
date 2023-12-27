#pragma once
#include "pico/stdlib.h"
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "i2cSensor.h"

#define BME680_REG_CHIP_ID      0xD0
#define BME680_REG_CTRL_MEASURE 0x73
#define BME680_REG_RESET        0xE0

#define BME680_CMD_RESET        0xB6

#define BME680_VALUE_CHIP_ID    0x61

/// @brief Class to handle communication with BME680
class BME680 : public I2CSensor {
public:
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;
    float voc = 0;
    uint64_t lastUpdated;

    /// @brief Constructor
    /// @param initialise Initialise BME680 automatically. Defaults to true
    BME680(bool initialise = true) : I2CSensor(BME680_ADDRESS) {
        if (initialise) init();
    }

    bool init(const uint attempts = 3);
    bool updateData();
};
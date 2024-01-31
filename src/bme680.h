#pragma once
#include <pico/stdlib.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "bme68x/bme68x.hpp"
#include "i2cSensor.h"

#define BME680_REG_CHIP_ID      0xD0
#define BME680_REG_CTRL_MEASURE 0x73
#define BME680_REG_RESET        0xE0

#define BME680_CMD_RESET        0xB6

#define BME680_VALUE_CHIP_ID    0x61

/// @brief High-level library (wrapper around BME68X)
class BME680 : public I2CSensor {
protected:
    BME68X* bme68x = new BME68X();
public:
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;
    float gasResistance = 0;
    uint8_t gasIndex = 0;
    uint8_t measureIndex = 0;
    uint32_t timeTaken = 0;
    uint8_t readStatus = 0;

    /// @brief Constructor
    /// @param initialise Initialise BME680 automatically. Defaults to true
    BME680(bool initialise = true) : I2CSensor(BME680_ADDRESS) {
        if (initialise) init();
    }

    bool init(const uint attempts = 3);
    bool updateData(uint16_t heater_temp, uint16_t heater_duration);
    bool updateData() {
        return this->updateData(300, 100);
    }

    bool configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp);

    sensorData_t getData();
};
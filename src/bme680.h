#pragma once
#include <pico/stdlib.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "BoschBME68XApi/bme68x.h"
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

    struct bme68x_dev bme680;
    struct bme68x_conf bme680_conf;
    struct bme68x_heatr_conf bme680_heater_conf;

    /// @brief Constructor
    /// @param initialise Initialise BME680 automatically. Defaults to true
    BME680(bool initialise = true) : I2CSensor(BME680_ADDRESS) {
        if (initialise) init();
    }

    bool init(const uint attempts = 3);
    bool updateData();

    bool setTemperatureOversampling(uint8_t os);
    bool setPressureOversampling(uint8_t os);
    bool setHumidityOversampling(uint8_t os);
    bool setIIRFilterSize(uint8_t fs);
    bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);
    bool setODR(uint8_t odr);

    /// @brief Read register of sensor, wrapper around I2C::readRegister
    int readRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit = false) {
        if (!initialised && !ignoreInit) return -1;
        return I2C::readRegister(BME680_ADDRESS, reg, buf, len);
    }

    /// @brief Write to register of sensor, wrapper around I2C::writeRegister
    int writeRegister(const uint8_t reg, const uint8_t* buf, const size_t len, const bool ignoreInit = false) {
        if (!initialised && !ignoreInit) return -1;
        return I2C::writeRegister(BME680_ADDRESS, reg, buf, len);
    }
};

int8_t bme680_i2c_read(uint8_t regAddr, uint8_t* regBuf, uint32_t len, void* extraData);
int8_t bme680_i2c_write(uint8_t regAddr, const uint8_t* regBuf, uint32_t len, void* extraData);

static void delay_usec(uint32_t us, void* intf_ptr);
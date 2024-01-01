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
    uint32_t pressure = 0;
    uint32_t gas_resistance = 0;
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
    bool updateData(uint16_t heater_temp = 300, uint16_t heater_duration = 100);
    bool configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp);

    static int8_t i2c_read(uint8_t regAddr, uint8_t* regBuf, uint32_t len, void* bmePtr) {
        BME680* bme = (BME680*)bmePtr;

        int8_t result = i2c_write_blocking(I2C_PORT, BME680_ADDRESS, &regAddr, 1, true);
        result = i2c_read_blocking(I2C_PORT, BME680_ADDRESS, regBuf, len, false);

        return result == PICO_ERROR_GENERIC ? 1 : 0;
    }

    static int8_t i2c_write(uint8_t regAddr, uint8_t* regBuf, uint32_t len, void* bmePtr) {
        BME680* bme = (BME680*)bmePtr;

        uint8_t buffer[len + 1];
        buffer[0] = regAddr;
        for (auto x = 0u; x < len; x++) {
            buffer[x + 1] = regBuf[x];
        }

        int8_t result = i2c_write_blocking(I2C_PORT, BME680_ADDRESS, buffer, len + 1, false); // +1 because need to write address as well

        return result == PICO_ERROR_GENERIC ? 1 : 0;
    }

    static void delay_usec(uint32_t us, void* intf_ptr) {
        (void)intf_ptr; // Unused parameter
        sleep_us(us);
    }

    static void bme68x_check_rslt(const char api_name[], int8_t rslt) {
        if (!VERBOSE_PRINT) return;
        switch (rslt) {
        case BME68X_OK:
            /* Do nothing */
            break;
        case BME68X_E_NULL_PTR:
            printf("%s: Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            printf("%s: Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            printf("%s: Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            printf("%s: Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            printf("%s: Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            printf("%s: Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            printf("%s: Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
        }
    }
};
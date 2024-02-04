/*
  BME680 mid-layer API.
  Borrowed from https://github.com/pimoroni/pimoroni-pico/tree/main/drivers/bme68x
  with a few changes for FreeRTOS support and letting it work standalone (without 
  other Pimoroni libraries).
*/

#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "BoschBME68XApi/bme68x.h"
#include "BoschBME68XApi/bme68x_defs.h"
#include "stdio.h"

#include "config.h"
#include "globals.h"

class BME68X {
public:
    bme68x_dev device;
    bme68x_conf conf;
    bme68x_heatr_conf heatr_conf;

    bool init();
    bool configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp);
    bool read_forced(bme68x_data* data, uint16_t heater_temp = 300, uint16_t heater_duration = 100);
    bool read_parallel(bme68x_data* results, uint16_t* profile_temps, uint16_t* profile_durations, size_t profile_length);

    // Bindings for bme68x_dev 
    static int write_bytes(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void* intf_ptr) {
        uint8_t buffer[length + 1];
        buffer[0] = reg_addr;
        for (auto x = 0u; x < length; x++) {
            buffer[x + 1] = reg_data[x];
        }

        int result = i2c_write_timeout_per_char_us(I2C_PORT, BME680_ADDRESS, buffer, length + 1, false, I2C_PER_CHAR_TIMEOUT_US);

        return result == PICO_ERROR_GENERIC ? 1 : 0;
    };

    static int read_bytes(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void* intf_ptr) {
        int result = i2c_write_timeout_per_char_us(I2C_PORT, BME680_ADDRESS, &reg_addr, 1, true, I2C_PER_CHAR_TIMEOUT_US);
        result = i2c_read_timeout_per_char_us(I2C_PORT, BME680_ADDRESS, reg_data, length, false, I2C_PER_CHAR_TIMEOUT_US);

        return result == PICO_ERROR_GENERIC ? 1 : 0;
    };

    static void delay_us(uint32_t period, void* intf_ptr) {
        sleep_us(period);
    }

    /* From BME68X API examples/common/common.c */
    void bme68x_check_rslt(const char api_name[], int8_t rslt) {
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

private:
    int8_t address = BME680_ADDRESS;
};
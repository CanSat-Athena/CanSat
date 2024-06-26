/*
  BME680/688 mid-layer API.
  Borrowed from https://github.com/pimoroni/pimoroni-pico/tree/main/drivers/bme68x
  with a few changes for FreeRTOS support and letting it work standalone (without
  other Pimoroni libraries).
*/

#include "bme68x.hpp"
#include "pico/stdlib.h"

#include "i2c.h"

bool BME68X::init() {
    int8_t result = 0;

    device.intf = bme68x_intf::BME68X_I2C_INTF;
    device.read = (bme68x_read_fptr_t)&read_bytes;
    device.write = (bme68x_write_fptr_t)&write_bytes;
    device.delay_us = (bme68x_delay_us_fptr_t)&delay_us;
    device.amb_temp = 20;

    result = bme68x_init(&device);
    bme68x_check_rslt("bme68x_init", result);
    if (result != BME68X_OK) return false;

    result = bme68x_get_conf(&conf, &device);
    bme68x_check_rslt("bme68x_get_conf", result);
    if (result != BME68X_OK) return false;

    configure(BME68X_FILTER_OFF, BME68X_ODR_NONE, BME68X_OS_16X, BME68X_OS_1X, BME68X_OS_2X);

    return true;
}

bool BME68X::configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp) {
    int8_t result = 0;

    conf.filter = filter;
    conf.odr = odr;
    conf.os_hum = os_humidity;
    conf.os_pres = os_pressure;
    conf.os_temp = os_temp;

    bme68x_set_conf(&conf, &device);
    bme68x_check_rslt("bme68x_set_conf", result);
    if (result != BME68X_OK) return false;

    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp = 300;
    heatr_conf.heatr_dur = 100;

    result = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &device);
    bme68x_check_rslt("bme68x_set_heatr_conf", result);
    if (result != BME68X_OK) return false;

    return true;
}

bool BME68X::read_forced(bme68x_data* data) {
    int8_t result = 0;
    uint8_t n_fields;
    uint32_t delay_period;

    I2C::take();
    result = bme68x_set_op_mode(BME68X_FORCED_MODE, &device);
    bme68x_check_rslt("bme68x_set_op_mode", result);
    if (result != BME68X_OK) {
        I2C::give();
        return false;
    }

    delay_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &device) + (heatr_conf.heatr_dur * 1000);
    I2C::give();

    vTaskDelay(delay_period / 1000 + 1);

    for (int i = 0; i < 5; i++) {
        I2C::take();
        result = bme68x_get_data(BME68X_FORCED_MODE, data, &n_fields, &device);
        I2C::give();
        bme68x_check_rslt("bme68x_get_data", result);
        if (result == BME68X_OK) break;
        vTaskDelay(1);
    }
    if (result != BME68X_OK) return false;

    return true;
}

/*
Will read profile_length results with the given temperatures and duration multipliers into the results array.
Blocks until it has a valid result for each temp/duration, and returns the entire set in the given order.
*/
bool BME68X::read_parallel(bme68x_data* results, uint16_t* profile_temps, uint16_t* profile_durations, size_t profile_length) {
    int8_t result;
    bme68x_data data[3]; // Parallel & Sequential mode read 3 simultaneous fields
    uint8_t n_fields;
    uint32_t delay_period;

    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp_prof = profile_temps;
    heatr_conf.heatr_dur_prof = profile_durations;
    heatr_conf.profile_len = profile_length;
    heatr_conf.shared_heatr_dur = 140 - (bme68x_get_meas_dur(BME68X_PARALLEL_MODE, &conf, &device) / 1000);
    result = bme68x_set_heatr_conf(BME68X_PARALLEL_MODE, &heatr_conf, &device);
    bme68x_check_rslt("bme68x_set_heatr_conf", result);
    if (result != BME68X_OK) return false;

    result = bme68x_set_op_mode(BME68X_PARALLEL_MODE, &device);
    bme68x_check_rslt("bme68x_set_op_mode", result);
    if (result != BME68X_OK) return false;

    while (1) {
        delay_period = bme68x_get_meas_dur(BME68X_PARALLEL_MODE, &conf, &device) + (heatr_conf.shared_heatr_dur * 1000);
        device.delay_us(delay_period, device.intf_ptr);

        result = bme68x_get_data(BME68X_PARALLEL_MODE, data, &n_fields, &device);
        if (result == BME68X_W_NO_NEW_DATA) continue;
        bme68x_check_rslt("bme68x_get_data", result);
        if (result != BME68X_OK) return false;

        for (auto i = 0u; i < n_fields; i++) {
            results[data[i].gas_index] = data[i];

            if (data[i].gas_index == profile_length - 1) return true;
        }
    }

    return true;
}
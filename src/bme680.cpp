#include "bme680.h"

/// @brief Initialise BME680
/// @param attempts Number of times to retry connection before quitting. Defaults to: 3
/// @return True if connected, false if error
bool BME680::init(const uint attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(10);

    // Configure the BME680 api
    bme680.chip_id = BME680_ADDRESS;
    bme680.intf = BME68X_I2C_INTF;
    bme680.intf_ptr = (void*)this;
    bme680.read = (bme68x_read_fptr_t)&i2c_read;
    bme680.write = (bme68x_write_fptr_t)&i2c_write;

    bme680.amb_temp = 20;
    bme680.delay_us = (bme68x_delay_us_fptr_t)&delay_usec;

    int8_t result;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to BME680, attempt %d of %d\n", i + 1, attempts);

        result = bme68x_init(&bme680);
        bme68x_check_rslt("bme68x_init", result);
        // Re-attempt if error
        if (result != BME68X_OK) {
            goto retry;
        }

        result = bme68x_get_conf(&bme680_conf, &bme680);
        bme68x_check_rslt("bme68x_get_conf", result);
        // Re-attempt if error
        if (result != BME68X_OK) {
            goto retry;
        }

        printf("BME680 responded successfully\n");
        printf("Initialising settings\n");

        configure(BME68X_FILTER_OFF, BME68X_ODR_NONE, BME68X_OS_16X, BME68X_OS_1X, BME68X_OS_2X);

        if (VERBOSE_PRINT) {
            printf("T1 = ");
            printf("%d\n", bme680.calib.par_t1);
            printf("T2 = ");
            printf("%d\n", bme680.calib.par_t2);
            printf("T3 = ");
            printf("%d\n", bme680.calib.par_t3);
            printf("P1 = ");
            printf("%d\n", bme680.calib.par_p1);
            printf("P2 = ");
            printf("%d\n", bme680.calib.par_p2);
            printf("P3 = ");
            printf("%d\n", bme680.calib.par_p3);
            printf("P4 = ");
            printf("%d\n", bme680.calib.par_p4);
            printf("P5 = ");
            printf("%d\n", bme680.calib.par_p5);
            printf("P6 = ");
            printf("%d\n", bme680.calib.par_p6);
            printf("P7 = ");
            printf("%d\n", bme680.calib.par_p7);
            printf("P8 = ");
            printf("%d\n", bme680.calib.par_p8);
            printf("P9 = ");
            printf("%d\n", bme680.calib.par_p9);
            printf("P10 = ");
            printf("%d\n", bme680.calib.par_p10);
            printf("H1 = ");
            printf("%d\n", bme680.calib.par_h1);
            printf("H2 = ");
            printf("%d\n", bme680.calib.par_h2);
            printf("H3 = ");
            printf("%d\n", bme680.calib.par_h3);
            printf("H4 = ");
            printf("%d\n", bme680.calib.par_h4);
            printf("H5 = ");
            printf("%d\n", bme680.calib.par_h5);
            printf("H6 = ");
            printf("%d\n", bme680.calib.par_h6);
            printf("H7 = ");
            printf("%d\n", bme680.calib.par_h7);
            printf("G1 = ");
            printf("%d\n", bme680.calib.par_gh1);
            printf("G2 = ");
            printf("%d\n", bme680.calib.par_gh2);
            printf("G3 = ");
            printf("%d\n", bme680.calib.par_gh3);
            printf("G1 = ");
            printf("%d\n", bme680.calib.par_gh1);
            printf("G2 = ");
            printf("%d\n", bme680.calib.par_gh2);
            printf("G3 = ");
            printf("%d\n", bme680.calib.par_gh3);
            printf("Heat Range = ");
            printf("%d\n", bme680.calib.res_heat_range);
            printf("Heat Val = ");
            printf("%d\n", bme680.calib.res_heat_val);
            printf("SW Error = ");
            printf("%d\n", bme680.calib.range_sw_err);
        }

        printf("Initialised BME680\n");
        this->initialised = true;

        return true;

    retry:
        // Delay and print retrying message if failed
        if (i < attempts - 1) {
            sleep_ms(2000);
            printf("Retrying...\n");
        }
    }

    printf("Failed to initialise BME680\n");

    return false;
}

bool BME680::updateData(uint16_t heater_temp, uint16_t heater_duration) {
    int8_t result = 0;
    uint8_t n_fields;
    uint32_t delayPeriod;
    struct bme68x_data *data;

    sleep_us(delayPeriod);

    bme680_heater_conf.enable = BME68X_ENABLE;
    bme680_heater_conf.heatr_temp = heater_temp;
    bme680_heater_conf.heatr_dur = heater_duration;

    result = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &bme680_heater_conf, &bme680);
    bme68x_check_rslt("bme68x_set_heatr_conf", result);
    if (result != BME68X_OK) return false;

    result = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme680);
    bme68x_check_rslt("bme68x_set_op_mode", result);
    if (result != BME68X_OK) return false;

    delayPeriod = bme68x_get_meas_dur(BME68X_FORCED_MODE, &bme680_conf, &bme680) + (bme680_heater_conf.heatr_dur * 1000);
    sleep_us(delayPeriod);

    result = bme68x_get_data(BME68X_FORCED_MODE, data, &n_fields, &bme680);
    bme68x_check_rslt("bme68x_get_data", result);
    if (result != BME68X_OK) return false;

    if (result != BME68X_OK)
        return false;

    if (n_fields) {
        temperature = data->temperature;
        humidity = data->humidity;
        pressure = data->pressure;

        if (data->status & (BME68X_HEAT_STAB_MSK | BME68X_GASM_VALID_MSK)) {
            gas_resistance = data->gas_resistance;
        } else {
            gas_resistance = 0;
        }
    }

    return true;
}

bool BME680::configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp) {
    int8_t result = 0;

    bme680_conf.filter = filter;
    bme680_conf.odr = odr;
    bme680_conf.os_hum = os_humidity;
    bme680_conf.os_pres = os_pressure;
    bme680_conf.os_temp = os_temp;

    bme68x_set_conf(&bme680_conf, &bme680);
    bme68x_check_rslt("bme68x_set_conf", result);

    if (result != BME68X_OK) return false;

    return true;
}
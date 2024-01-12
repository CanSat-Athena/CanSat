#include "bme680.h"

/// @brief Initialise BME680
/// @param attempts Number of times to retry connection before quitting. Defaults to: 3
/// @return True if connected, false if error
bool BME680::init(const uint attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(10);

    int8_t result;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to BME680, attempt %d of %d\n", i + 1, attempts);

        result = bme68x->init();
        // Re-attempt if error
        if (!result) {
            printf("Failed at BME680 init()\n");
            goto retry;
        }

        result = bme68x_get_conf(&bme68x->conf, &bme68x->device);
        bme68x->bme68x_check_rslt("bme68x_get_conf", result);
        // Re-attempt if error
        if (result != BME68X_OK) {
            printf("Failed at bme68x_get_conf()\n");
            goto retry;
        }

        printf("BME680 responded successfully\n");
        printf("Initialising settings\n");

        configure(BME68X_FILTER_OFF, BME68X_ODR_NONE, BME68X_OS_16X, BME68X_OS_1X, BME68X_OS_2X);

        if (EXTRA_VERBOSE_BME680) {
            printf("T1 = ");
            printf("%d\n", bme68x->device.calib.par_t1);
            printf("T2 = ");
            printf("%d\n", bme68x->device.calib.par_t2);
            printf("T3 = ");
            printf("%d\n", bme68x->device.calib.par_t3);
            printf("P1 = ");
            printf("%d\n", bme68x->device.calib.par_p1);
            printf("P2 = ");
            printf("%d\n", bme68x->device.calib.par_p2);
            printf("P3 = ");
            printf("%d\n", bme68x->device.calib.par_p3);
            printf("P4 = ");
            printf("%d\n", bme68x->device.calib.par_p4);
            printf("P5 = ");
            printf("%d\n", bme68x->device.calib.par_p5);
            printf("P6 = ");
            printf("%d\n", bme68x->device.calib.par_p6);
            printf("P7 = ");
            printf("%d\n", bme68x->device.calib.par_p7);
            printf("P8 = ");
            printf("%d\n", bme68x->device.calib.par_p8);
            printf("P9 = ");
            printf("%d\n", bme68x->device.calib.par_p9);
            printf("P10 = ");
            printf("%d\n", bme68x->device.calib.par_p10);
            printf("H1 = ");
            printf("%d\n", bme68x->device.calib.par_h1);
            printf("H2 = ");
            printf("%d\n", bme68x->device.calib.par_h2);
            printf("H3 = ");
            printf("%d\n", bme68x->device.calib.par_h3);
            printf("H4 = ");
            printf("%d\n", bme68x->device.calib.par_h4);
            printf("H5 = ");
            printf("%d\n", bme68x->device.calib.par_h5);
            printf("H6 = ");
            printf("%d\n", bme68x->device.calib.par_h6);
            printf("H7 = ");
            printf("%d\n", bme68x->device.calib.par_h7);
            printf("G1 = ");
            printf("%d\n", bme68x->device.calib.par_gh1);
            printf("G2 = ");
            printf("%d\n", bme68x->device.calib.par_gh2);
            printf("G3 = ");
            printf("%d\n", bme68x->device.calib.par_gh3);
            printf("G1 = ");
            printf("%d\n", bme68x->device.calib.par_gh1);
            printf("G2 = ");
            printf("%d\n", bme68x->device.calib.par_gh2);
            printf("G3 = ");
            printf("%d\n", bme68x->device.calib.par_gh3);
            printf("Heat Range = ");
            printf("%d\n", bme68x->device.calib.res_heat_range);
            printf("Heat Val = ");
            printf("%d\n", bme68x->device.calib.res_heat_val);
            printf("SW Error = ");
            printf("%d\n", bme68x->device.calib.range_sw_err);
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
    if (!initialised) return false;
    
    absolute_time_t absoluteTime = get_absolute_time();
    uint32_t startOfMeasurement = to_ms_since_boot(absoluteTime);

    bme68x_data data;

    // Take measurement
    bool result = bme68x->read_forced(&data);
    if (!result) return false;

    absoluteTime = get_absolute_time();
    lastUpdated = to_ms_since_boot(absoluteTime);

    this->lastUpdated = lastUpdated;
    this->temperature = data.temperature;
    this->pressure = data.pressure;
    this->humidity = data.humidity;
    this->gasResistance = data.gas_resistance;
    this->readStatus = data.status;
    this->gasIndex = data.gas_index;
    this->measureIndex = data.meas_index;
    this->timeTaken = lastUpdated - startOfMeasurement;

    return true;
}

bool BME680::configure(uint8_t filter, uint8_t odr, uint8_t os_humidity, uint8_t os_pressure, uint8_t os_temp) {
    return bme68x->configure(filter, odr, os_humidity, os_pressure, os_temp);
}
#include "bme680.h"
#include "streamHandler.h"

/// @brief Initialise BME680
/// @param attempts Number of times to retry connection before quitting. Defaults to: 3
/// @return True if connected, false if error
bool BME680::init(const uint attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(10);

    int8_t result;

    for (int i = 0; i < attempts; i++) {
        StreamHandler::tPrintf("BME680:     Trying to connect, attempt %d of %d\n", i + 1, attempts);

        result = bme68x->init();
        // Re-attempt if error
        if (!result) {
            StreamHandler::tPrintf("BME680:     Failed at init()\n");
            goto retry;
        }

        result = bme68x_get_conf(&bme68x->conf, &bme68x->device);
        bme68x->bme68x_check_rslt("bme68x_get_conf", result);
        // Re-attempt if error
        if (result != BME68X_OK) {
            StreamHandler::tPrintf("BME680:     Failed at bme68x_get_conf()\n");
            goto retry;
        }

        StreamHandler::tPrintf("BME680:     Responded successfully\n");
        StreamHandler::tPrintf("BME680:     Initialising settings\n");

        configure(BME68X_FILTER_OFF, BME68X_ODR_NONE, BME68X_OS_16X, BME68X_OS_1X, BME68X_OS_2X);

        if (EXTRA_VERBOSE_BME680) {
            StreamHandler::tPrintf("T1 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_t1);
            StreamHandler::tPrintf("T2 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_t2);
            StreamHandler::tPrintf("T3 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_t3);
            StreamHandler::tPrintf("P1 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p1);
            StreamHandler::tPrintf("P2 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p2);
            StreamHandler::tPrintf("P3 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p3);
            StreamHandler::tPrintf("P4 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p4);
            StreamHandler::tPrintf("P5 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p5);
            StreamHandler::tPrintf("P6 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p6);
            StreamHandler::tPrintf("P7 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p7);
            StreamHandler::tPrintf("P8 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p8);
            StreamHandler::tPrintf("P9 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p9);
            StreamHandler::tPrintf("P10 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_p10);
            StreamHandler::tPrintf("H1 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h1);
            StreamHandler::tPrintf("H2 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h2);
            StreamHandler::tPrintf("H3 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h3);
            StreamHandler::tPrintf("H4 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h4);
            StreamHandler::tPrintf("H5 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h5);
            StreamHandler::tPrintf("H6 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h6);
            StreamHandler::tPrintf("H7 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_h7);
            StreamHandler::tPrintf("G1 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_gh1);
            StreamHandler::tPrintf("G2 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_gh2);
            StreamHandler::tPrintf("G3 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_gh3);
            StreamHandler::tPrintf("G1 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_gh1);
            StreamHandler::tPrintf("G2 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_gh2);
            StreamHandler::tPrintf("G3 = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.par_gh3);
            StreamHandler::tPrintf("Heat Range = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.res_heat_range);
            StreamHandler::tPrintf("Heat Val = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.res_heat_val);
            StreamHandler::tPrintf("SW Error = ");
            StreamHandler::tPrintf("%d\n", bme68x->device.calib.range_sw_err);
        }

        StreamHandler::tPrintf("BME680:     Initialised successfully\n");
        this->initialised = true;

        return true;

    retry:
        // Delay and print retrying message if failed
        if (i < attempts - 1) {
            sleep_ms(2000);
            StreamHandler::tPrintf("BME680:     Retrying...\n");
        }
    }

    StreamHandler::tPrintf("BME680:     Failed to initialise\n");

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

    // absoluteTime = get_absolute_time();
    // this->lastUpdated = to_ms_since_boot(absoluteTime);

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

sensorData_t BME680::getData() {
    return {
        .bme680 = bme680Data_t {
            .temperature = this->temperature,
            .humidity = this->humidity,
            .pressure = this->pressure,
            .gasResistance = this->gasResistance,
            // .gasIndex = this->gasIndex,
            // .measureIndex = this->measureIndex,
            // .readStatus = this->readStatus,
        }
    };
}
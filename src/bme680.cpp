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
    bme680.read = &bme680_i2c_read;
    bme680.write = &bme680_i2c_write;

    bme680.amb_temp = 25;
    bme680.delay_us = delay_usec;

    int8_t result;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to BME680, attempt %d of %d\n", i + 1, attempts);

        result = bme68x_init(&bme680);

        // Re-attempt if different value
        if (result != BME68X_OK) {
            printf("Failed to connect to BME680, received error: %d\n", result);
            goto retry;
        }

        printf("BME680 responded successfully\n");
        printf("Initialising settings\n");

        setIIRFilterSize(BME68X_FILTER_SIZE_3);
        setODR(BME68X_ODR_NONE);
        setHumidityOversampling(BME68X_OS_2X);
        setPressureOversampling(BME68X_OS_4X);
        setTemperatureOversampling(BME68X_OS_8X);
        setGasHeater(320, 150); // 320*C for 150 ms

        result = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme680);

        // Re-attempt if result is not BME68X_OK
        if (result != BME68X_OK) {
            printf("Failed to initialise BME680, received error: %d\n", result);
            goto retry;
        }

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

bool BME680::updateData() {
    return false;
}

/*!
 *  @brief  Setter for temperature oversampling, borrowed from Adafruit BME680 library
 *  @param  oversample
 *          Oversampling setting, can be BME68X_OS_NONE (turn off temperature
 * reading), BME68X_OS_1X, BME68X_OS_2X, BME68X_OS_4X, BME68X_OS_8X or
 * BME68X_OS_16X
 *  @return True on success, false on failure
 */
bool BME680::setTemperatureOversampling(uint8_t oversample) {
    if (oversample > BME68X_OS_16X)
        return false;

    bme680_conf.os_temp = oversample;

    int8_t rslt = bme68x_set_conf(&bme680_conf, &bme680);
    return rslt == 0;
}

/*!
 *  @brief  Setter for humidity oversampling, borrowed from Adafruit BME680 library
 *  @param  oversample
 *          Oversampling setting, can be BME68X_OS_NONE (turn off humidity
 * reading), BME68X_OS_1X, BME68X_OS_2X, BME68X_OS_4X, BME68X_OS_8X or
 * BME68X_OS_16X
 *
 * More oversampling = higher power usage
 *  @return True on success, false on failure
 */
bool BME680::setHumidityOversampling(uint8_t oversample) {
    if (oversample > BME68X_OS_16X)
        return false;

    bme680_conf.os_hum = oversample;

    int8_t rslt = bme68x_set_conf(&bme680_conf, &bme680);
    return rslt == 0;
}

/*!
 *  @brief  Setter for pressure oversampling, borrowed from Adafruit BME680 library
 *  @param  oversample
 *          Oversampling setting, can be BME68X_OS_NONE (turn off pressure
 * reading), BME68X_OS_1X, BME68X_OS_2X, BME68X_OS_4X, BME68X_OS_8X or
 * BME68X_OS_16X
 *  @return True on success, false on failure
 */
bool BME680::setPressureOversampling(uint8_t oversample) {
    if (oversample > BME68X_OS_16X)
        return false;

    bme680_conf.os_pres = oversample;

    int8_t rslt = bme68x_set_conf(&bme680_conf, &bme680);
    return rslt == 0;
}

/*!
 *  @brief  Enable and configure gas reading + heater, borrowed from Adafruit BME680 library
 *  @param  heaterTemp Desired temperature in degrees Centigrade
 *  @param  heaterTime Time to keep heater on in milliseconds
 *  @return True on success, false on failure
 */
bool BME680::setGasHeater(uint16_t heaterTemp, uint16_t heaterTime) {
    if ((heaterTemp == 0) || (heaterTime == 0)) {
        bme680_heater_conf.enable = BME68X_DISABLE;
    } else {
        bme680_heater_conf.enable = BME68X_ENABLE;
        bme680_heater_conf.heatr_temp = heaterTemp;
        bme680_heater_conf.heatr_dur = heaterTime;
    }

    int8_t rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &bme680_heater_conf, &bme680);
    return rslt == 0;
}

/*!
 *  @brief  Setter for Output Data Rate, borrowed from Adafruit BME680 library
 *  @param  odr
 *          Output data rate setting, can be BME68X_ODR_NONE,
 * BME68X_ODR_0_59_MS, BME68X_ODR_10_MS, BME68X_ODR_20_MS, BME68X_ODR_62_5_MS,
 * BME68X_ODR_125_MS, BME68X_ODR_250_MS, BME68X_ODR_500_MS, BME68X_ODR_1000_MS
 *  @return True on success, False on failure
 */
bool BME680::setODR(uint8_t odr) {
    if (odr > BME68X_ODR_NONE)
        return false;

    bme680_conf.odr = odr;

    int8_t rslt = bme68x_set_conf(&bme680_conf, &bme680);
    return rslt == 0;
}

/*!
 *  @brief  Setter for IIR filter, borrowed from Adafruit BME680 library
 *  @param  filterSize Size of the filter (in samples).
 *          Can be BME68X_FILTER_SIZE_0 (no filtering), BME68X_FILTER_SIZE_1,
 * BME68X_FILTER_SIZE_3, BME68X_FILTER_SIZE_7, BME68X_FILTER_SIZE_15,
 * BME68X_FILTER_SIZE_31, BME68X_FILTER_SIZE_63, BME68X_FILTER_SIZE_127
 *  @return True on success, false on failure
 */
bool BME680::setIIRFilterSize(uint8_t filterSize) {
    if (filterSize > BME68X_FILTER_SIZE_127)
        return false;

    bme680_conf.filter = filterSize;

    int8_t rslt = bme68x_set_conf(&bme680_conf, &bme680);
    return rslt == 0;
}


int8_t bme680_i2c_read(uint8_t regAddr, uint8_t* regBuf, uint32_t len, void* bmePtr) {
    BME680* bme = (BME680*)bmePtr;

    int8_t result = bme->readRegister(regAddr, regBuf, len, true);      // Ignore init

    if (result == len) {
        return 0;
    }

    // Error
    return -1;
}

int8_t bme680_i2c_write(uint8_t regAddr, const uint8_t* regBuf, uint32_t len, void* bmePtr) {
    BME680* bme = (BME680*)bmePtr;

    int8_t result = bme->writeRegister(regAddr, regBuf, len, true);       // Ignore init

    if (result == len) {
        return 0;
    }

    // Error
    return -1;
}

static void delay_usec(uint32_t us, void* intf_ptr) {
    (void)intf_ptr; // Unused parameter
    sleep_us(us);
}

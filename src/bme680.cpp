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

static void delay_usec(uint32_t us, void *intf_ptr) {
  (void)intf_ptr; // Unused parameter
  sleep_us(us);
}

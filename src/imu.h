#pragma once
#include <pico/stdio.h>
#include <FreeRTOS.h>

extern "C" {
#include "pico-icm20948/src/pico-icm20948.h"
}

#include "i2cSensor.h"
#include "commonTypes.h"
#include "config.h"

class IMU : public I2CSensor {
private:
    icm20948_config_t config = { IMU_ADDRESS, 0x0C, &I2C::i2cInstance };
    icm20984_data_t data;

public:
    int16_t accel_raw[3] = { 0 }, gyro_raw[3] = { 0 }, mag_raw[3] = { 0 }, temp_raw = 0;
    float accel_g[3] = { 0 }, gyro_dps[3] = { 0 }, mag_ut[3] = { 0 }, temp_c = 0;

    IMU(bool initialise = true) : I2CSensor(IMU_ADDRESS) {
        if (initialise) init();
    }

    /// @brief Initialises the ICM-20948
    /// @param attempts Number of attempts to retry for until failing
    /// @return Failed or success
    bool init(const int attempts = 3);
};
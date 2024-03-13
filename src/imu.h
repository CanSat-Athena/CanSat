#pragma once
#include <pico/stdio.h>
#include <FreeRTOS.h>
#include <task.h>

extern "C" {
#include "pico-icm20948/src/pico-icm20948.h"
}

#include "i2cSensor.h"
#include "commonTypes.h"
#include "config.h"

class IMU : public I2CSensor {
private:
    icm20948_config_t config = { IMU_ADDRESS, 0x0C, vTaskDelay, I2C::take, I2C::give, &I2C::i2cInstance };
    icm20984_data_t data;

public:
    int16_t accel[3] = { 0 }, gyro[3] = { 0 }, mag[3] = { 0 };
    float temp_c = 0;

    IMU(bool initialise = true) : I2CSensor(IMU_ADDRESS) {
        if (initialise) init();
    }

    bool init(const int attempts = 1);
    bool updateData();
    sensorData_t getData();
};
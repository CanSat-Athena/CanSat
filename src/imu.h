#pragma once
#include <pico/stdio.h>
#include <FreeRTOS.h>

#include "i2cSensor.h"
#include "commonTypes.h"
#include "config.h"

#define IMU_BANK_REG 0x7F   // 127

class IMU : public I2CSensor {
private:
    uint8_t bank = 0;
public:
    IMU(bool initialise = true) : I2CSensor(IMU_ADDRESS) {
        if (initialise) init();
    }

    /// @brief Initialises the ICM-20948
    /// @param attempts Number of attempts to retry for until failing
    /// @return Failed or success
    bool init(const int attempts = 3);

    /// @brief Set the register bank bits of IMU
    /// @param regBank The bank number
    /// @return True on success
    bool setRegisterBank(const uint8_t regBank);

    bool reset();
};
#include "mpu6050.h"

MPU6050::MPU6050(const bool initialise) {
    if (initialise) init();
}

/// @brief Initialise MPU6050
/// @param attempts Number of times to retry connection before quitting. Defaults to: 3
/// @return True if connected, false if error
bool MPU6050::init(const uint attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(250);

    uint8_t whoamiRegisterValue = 0x00;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to MPU6050, attempt %d of %d\n", i + 1, attempts);
        readRegister(WHOAMI_REG, &whoamiRegisterValue, 1, true);

        if (whoamiRegisterValue == MPU6050_ADDRESS) {
            printf("MPU-6050 initialised; I2C address: 0x%x\n", whoamiRegisterValue);

            this->initialised = true;
            return true;
        }

        printf("Failed to connect, read I2C address: 0x%x\n", whoamiRegisterValue);

        sleep_ms(2000);

        if (i < attempts - 1)
            printf("Retrying...\n");
    }

    printf("Failed to initialise MPU-6050\n");

    return false;
}

/// @brief Read register of MPU6050, wrapper around I2C::readRegister
int MPU6050::readRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit) {
    if (!initialised && !ignoreInit) return -1;
    return I2C::readRegister(MPU6050_ADDRESS, reg, buf, len);
}
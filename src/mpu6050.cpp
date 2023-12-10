#include "mpu6050.h"

MPU6050::MPU6050(const bool initialise) {
    if (initialise) init();
}

void MPU6050::init() {
    if (initialised) return;

    // Wait for sensor to start up
     sleep_ms(250);

    uint8_t whoamiRegisterValue = 0x00;
    uint8_t whoamiAddress = WHOAMI_REG;
    // I2C::readRegister(MPU6050_ADDRESS, WHOAMI_REG, whoamiRegisterValue, 1);
    i2c_write_blocking(I2C_PORT, MPU6050_ADDRESS, &whoamiAddress, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDRESS, &whoamiRegisterValue, 1, false);
    printf("MPU-6050 I2C address: 0x%x\n", whoamiRegisterValue);

    this->initialised = true;
}
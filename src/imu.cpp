#include "imu.h"

bool IMU::init(const int attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(100);

    for (int i = 0; i < attempts; i++) {
        printf("IMU:        Trying to connect, attempt %d of %d\n", i + 1, attempts);

        
        // printf("IMU:        Initialised successfully; I2C status: 0x%x\n", status);

        this->initialised = true;
        return true;

    retry:
        // Delay and print retrying message if failed
        if (i < attempts - 1) {
            sleep_ms(2000);
            printf("IMU:        Retrying...\n");
        }
    }

    printf("IMU:        Failed to initialise\n");

    return false;
}

bool IMU::setRegisterBank(const uint8_t regBank) {
    
    return true;
}

bool IMU::reset() {

    return true;
}
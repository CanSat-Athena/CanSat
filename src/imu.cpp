#include "imu.h"

bool IMU::init(const int attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(100);

    for (int i = 0; i < attempts; i++) {
        printf("IMU:        Trying to connect, attempt %d of %d\n", i + 1, attempts);

        if (icm20948_init(&config) != 0) goto retry;

        printf("IMU:        Initialised successfully");
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
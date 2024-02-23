#include "imu.h"

/// @brief Initialises the ICM-20948
/// @param attempts Number of attempts to retry for until failing
/// @return Failed or success
bool IMU::init(const int attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(100);

    for (int i = 0; i < attempts; i++) {
        printf("IMU:        Trying to connect, attempt %d of %d\n", i + 1, attempts);

        if (icm20948_init(&config) != 0) goto retry;
        icm20948_set_mag_rate(&config, IMU_READ_FREQ);

        printf("IMU:        Initialised successfully\n");
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

/// @brief Updates the IMU data
/// @return True if successful
bool IMU::updateData() {
    icm20948_read_raw_accel(&config, accel_raw);
    icm20948_read_raw_gyro(&config, gyro_raw);
    icm20948_read_raw_mag(&config, mag_raw);
    icm20948_read_temp_c(&config, &temp_c);

    for (uint8_t i = 0; i < 3; i++) {
        accel_g[i] = (float)accel_raw[i] / 16384.0f;
        gyro_dps[i] = (float)gyro_raw[i] / 131.0f;
        mag_ut[i] = ((float)mag_raw[i] / 20) * 3;
    }

    // printf("accel. x: %+2.5f, y: %+2.5f, z:%+2.5f\n", accel_g[0], accel_g[1], accel_g[2]);
    // printf("gyro.  x: %+2.5f, y: %+2.5f, z:%+2.5f\n", gyro_dps[0], gyro_dps[1], gyro_dps[2]);
    // printf("mag.   x: %+2.5f, y: %+2.5f, z:%+2.5f\n", mag_ut[0], mag_ut[1], mag_ut[2]);
    // printf("temp: %+2.5f\n", temp_c);

    return true;
}
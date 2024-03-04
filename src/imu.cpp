#include "imu.h"
#include "streamHandler.h"

/// @brief Initialises the ICM-20948
/// @param attempts Number of attempts to retry for until failing
/// @return Failed or success
bool IMU::init(const int attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(100);

    for (int i = 0; i < attempts; i++) {
        StreamHandler::tPrintf("IMU:        Trying to connect, attempt %d of %d\n", i + 1, attempts);

        if (icm20948_init(&config) != 0) goto retry;
        icm20948_set_mag_rate(&config, max(IMU_READ_FREQ, 10));
        // icm20948_cal_gyro(&config, &data.gyro_bias[0]);
        // StreamHandler::tPrintf("IMU:        Calibrated gyro: %d %d %d\n", data.gyro_bias[0], data.gyro_bias[1], data.gyro_bias[2]);
        // icm20948_cal_accel(&config, &data.accel_bias[0]);
        // StreamHandler::tPrintf("IMU:        Calibrated accel: %d %d %d\n", data.accel_bias[0], data.accel_bias[1], data.accel_bias[2]);
        // icm20948_cal_mag_simple(&config, &data.mag_bias[0]);
        // StreamHandler::tPrintf("IMU:        Calibrated mag: %d %d %d\n", data.mag_bias[0], data.mag_bias[1], data.mag_bias[2]);

        StreamHandler::tPrintf("IMU:        Initialised successfully\n");
        this->initialised = true;
        return true;

    retry:
        // Delay and print retrying message if failed
        if (i < attempts - 1) {
            sleep_ms(2000);
            StreamHandler::tPrintf("IMU:        Retrying...\n");
        }
    }

    StreamHandler::tPrintf("IMU:        Failed to initialise\n");
    return false;
}

/// @brief Updates the IMU data
/// @return True if successful
bool IMU::updateData() {
    icm20948_read_raw_accel(&config, accel);
    icm20948_read_raw_gyro(&config, gyro);
    icm20948_read_raw_mag(&config, mag);
    icm20948_read_temp_c(&config, &temp_c);

    // for (uint8_t i = 0; i < 3; i++) {
    //     accel_g[i] = (float)accel_raw[i] / (16384.0f / 1);
    //     gyro_dps[i] = (float)gyro_raw[i] / 131.0f;
    //     mag_ut[i] = ((float)mag_raw[i] / 20) * 3;
    // }

    return true;
}

sensorData_t IMU::getData() {
    return {
        .imu = {
            accel[0] = this->accel[0],
            accel[1] = this->accel[1],
            accel[2] = this->accel[2],

            gyro[0] = this->gyro[0],
            gyro[1] = this->gyro[1],
            gyro[2] = this->gyro[2],

            mag[0] = this->mag[0],
            mag[1] = this->mag[1],
            mag[2] = this->mag[2],
        }
    };
}
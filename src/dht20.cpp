#include "dht20.h"

/// @brief Constructor
/// @param initialise Initialise DHT20 automatically. Defaults to true
DHT20::DHT20(const bool initialise) {
    if (initialise) init();
}

/// @brief Initialise DHT20
/// @param attempts Number of times to retry connection before quitting. Defaults to: 3
/// @return True if connected, false if error
bool DHT20::init(const uint attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(100);

    // Pre-declare variables because using goto in loop
    constexpr uint8_t calibrateCmd[] = { DHT20_CALIBRATE_CMD, 0x08, 0x00 };
    constexpr uint8_t softResetCmd = DHT20_SOFTRESET_CMD;
    uint8_t status;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to DHT20, attempt %d of %d\n", i + 1, attempts);

        // Perform soft reset
        printf("Attempting DHT20 reset\n");
        i2c_write_blocking(I2C_PORT, DHT20_ADDRESS, &softResetCmd, 1, false);
        sleep_ms(20);

        // Re-attempt if timeout
        if (!waitForProcessing(false)) {
            printf("DHT20 connection timeout\n");
            goto retry;
        }

        // Attempt calibration
        printf("Attempting calibration\n");
        i2c_write_blocking(I2C_PORT, DHT20_ADDRESS, calibrateCmd, 3, false);

        // Re-attempt if timeout
        if (!waitForProcessing(false)) {
            printf("DHT20 connection timeout\n");
            sleep_ms(2000);

            if (i < attempts - 1)
                printf("Retrying...\n");

            continue;
        }

        status = readStatus();

        // Re-attempt if not calibrated
        if (!(status & DHT20_STATUS_CALIBRATED)) {
            printf("DHT20 calibration error\n");
            goto retry;
        }

        printf("DHT20 responded successfully; I2C status: 0x%x\n", status);

        this->initialised = true;
        return true;

        // Delay and print retrying message if failed
    retry:
        sleep_ms(2000);

        if (i < attempts - 1)
            printf("Retrying...\n");
    }

    printf("Failed to initialise DHT20\n");

    return false;
}

/// @brief Reads sensor data
/// @param data Pointer to the `dhtData_t` struct to dump data into
/// @return True on success, false on error
bool DHT20::getEvent(dhtData_t* data) {
    if (!initialised) return false;

    // Trigger read
    constexpr uint8_t triggerCmd[] = { DHT20_READ_CMD, 0x33, 0x00 };
    i2c_write_blocking(I2C_PORT, DHT20_ADDRESS, triggerCmd, 3, false);

    // Wait for read
    if (!waitForProcessing()) {
        return false;
    }

    uint8_t readData[6];
    i2c_read_blocking(I2C_PORT, DHT20_ADDRESS, readData, 6, false);

    return true;
}

/// @brief Read register of DHT20, wrapper around I2C::readRegister
int DHT20::readRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit) {
    if (!initialised && !ignoreInit) return -1;
    return I2C::readRegister(DHT20_ADDRESS, reg, buf, len);
}

/// @brief Write to register of DHT20, wrapper around I2C::writeRegister
int DHT20::writeRegister(const uint8_t reg, uint8_t* buf, const size_t len, const bool ignoreInit) {
    if (!initialised && !ignoreInit) return -1;
    return I2C::writeRegister(DHT20_ADDRESS, reg, buf, len);
}

/// @brief Read status byte of DHT20
/// @return The status byte read
uint8_t DHT20::readStatus() {
    uint8_t read;

    // Return 0xFF if error
    if (!i2c_read_blocking(I2C_PORT, DHT20_ADDRESS, &read, 1, false)) {
        return 0xFF;
    }
    return read;
}

/// @brief Wait for DHT20 status busy pin to return to 0
/// @param useRTOSDelay Determines whether `vTaskDelay` or `sleep_ms` should be used - defaults to true
/// @return True on success, false if exceeded the I2C timeout
bool DHT20::waitForProcessing(bool useRTOSDelay) {
    const uint16_t maxCount = I2C_TIMEOUT_MS / 10;
    uint16_t count = 0;

    while (readStatus() & DHT20_STATUS_BUSY) {
        if (useRTOSDelay) vTaskDelay(10);
        else sleep_ms(10);
        count += 10;

        if (count > maxCount) return false;
    }

    return true;
}
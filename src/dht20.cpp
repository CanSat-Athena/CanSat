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

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to DHT20, attempt %d of %d\n", i + 1, attempts);

        // Perform soft reset
        printf("Attempting DHT20 reset\n");
        uint8_t softResetCmd = DHT20_SOFTRESET_CMD;
        i2c_write_blocking(I2C_PORT, DHT20_ADDRESS, &softResetCmd, 1, false);
        sleep_ms(20);

        // Re-attempt if timeout
        if (!waitForCommunication()) {
            printf("DHT20 connection timeout\n");
            sleep_ms(2000);

            if (i < attempts - 1)
                printf("Retrying...\n");

            continue;
        }

        // Attempt calibration
        printf("Attempting calibration\n");
        uint8_t calibrateCmd[] = { DHT20_CALIBRATE_CMD, 0x08, 0x00 };
        i2c_write_blocking(I2C_PORT, DHT20_ADDRESS, calibrateCmd, 3, false);

        // Re-attempt if timeout
        if (!waitForCommunication()) {
            printf("DHT20 connection timeout\n");
            sleep_ms(2000);

            if (i < attempts - 1)
                printf("Retrying...\n");

            continue;
        }

        // Re-attempt if not calibrated
        if (!(readStatus() & DHT20_STATUS_CALIBRATED)) {
            printf("DHT20 calibration error\n");
            sleep_ms(2000);

            if (i < attempts - 1)
                printf("Retrying...\n");

            continue;
        }


        uint8_t statuswordRegisterValue = 0x00;
        readRegister(DHT20_STATUSWORD_REG, &statuswordRegisterValue, 1, true);

        // Check if succeeded - if bit 3 is 1
        if (statuswordRegisterValue & 0b00001000) {
            printf("DHT20 responded successfully; I2C status: 0x%x\n", statuswordRegisterValue);

            this->initialised = true;
            return true;
        }

    }

    printf("Failed to initialise DHT20\n");

    return false;
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

bool DHT20::waitForCommunication() {
    const uint16_t maxCount = I2C_TIMEOUT_MS / 10;
    uint16_t count = 0;
    
    while (readStatus() & DHT20_STATUS_BUSY) {
        sleep_ms(10);
        count += 10;

        if (count > maxCount) return false;
    }

    return true;
}
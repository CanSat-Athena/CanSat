#include "bme680.h"

/// @brief Initialise BME680
/// @param attempts Number of times to retry connection before quitting. Defaults to: 3
/// @return True if connected, false if error
bool BME680::init(const uint attempts) {
    if (initialised) return false;

    // Wait for sensor to start up
    sleep_ms(10);

    // Pre-declare variables because using goto in loop
    constexpr uint8_t softResetCmd = BME680_CMD_RESET;
    uint8_t chipIdValue;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to BME680, attempt %d of %d\n", i + 1, attempts);

        // Perform soft reset
        printf("Attempting BME680 reset\n");
        writeRegister(BME680_REG_RESET, &softResetCmd, 1, true);
        sleep_ms(20);

        // Read CHIP_ID register - should be 0x61
        readRegister(BME680_REG_CHIP_ID, &chipIdValue, 1, true);

        // Re-attempt if different value
        if (chipIdValue != BME680_VALUE_CHIP_ID) {
            printf("BME680 connection timeout\n");
            goto retry;
        }

        printf("BME680 responded successfully; chip ID: 0x%x\n", chipIdValue);

        this->initialised = true;
        return true;

    retry:
        // Delay and print retrying message if failed
        if (i < attempts - 1) {
            sleep_ms(2000);
            printf("Retrying...\n");
        }
    }

    printf("Failed to initialise BME680\n");

    return false;
}


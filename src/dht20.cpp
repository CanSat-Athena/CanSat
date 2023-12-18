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

    uint8_t statuswordRegisterValue = 0x00;

    for (int i = 0; i < attempts; i++) {
        printf("Trying to connect to DHT20, attempt %d of %d\n", i + 1, attempts);
        readRegister(DHT20_STATUSWORD_REG, &statuswordRegisterValue, 1, true);

        // Check if succeeded - if bit 3 is 1
        if (statuswordRegisterValue & 0b00001000) {
            printf("DHT20 responded successfully; I2C address: 0x%x\n", statuswordRegisterValue);

            this->initialised = true;
            return true;
        } 
        // else {
        //     printf("Failed to connect, read I2C address value: 0x%x\n", statuswordRegisterValue);
        //     printf("Trying initialise\n");
        //     uint8_t writeBuf[] = {0x08, 0x00};
        //     writeRegister(DHT20_INITIALISE_REG, writeBuf, sizeof(writeBuf) / sizeof(uint8_t), true);
        // }

        sleep_ms(2000);

        if (i < attempts - 1)
            printf("Retrying...\n");
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
#include "dht20.h"

/// @brief Constructor
/// @param initialise Initialise DHT20 automatically. Defaults to true
DHT20::DHT20(const bool initialise) : I2CSensor(DHT20_ADDRESS) {
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
        printf("DHT20:      Trying to connect, attempt %d of %d\n", i + 1, attempts);

        // Perform soft reset
        printf("DHT20:      Attempting reset\n");
        i2c_write_timeout_per_char_us(I2C_PORT, i2cAddress, &softResetCmd, 1, false, I2C_PER_CHAR_TIMEOUT_US);
        sleep_ms(20);

        // Re-attempt if timeout
        if (!waitForProcessing(false)) {
            printf("DHT20:      Connection timeout\n");
            goto retry;
        }

        // Attempt calibration
        printf("DHT20:      Attempting calibration\n");
        i2c_write_timeout_per_char_us(I2C_PORT, i2cAddress, calibrateCmd, 3, false, I2C_PER_CHAR_TIMEOUT_US);

        // Re-attempt if timeout
        if (!waitForProcessing(false)) {
            printf("DHT20:      Connection timeout\n");
            sleep_ms(2000);

            if (i < attempts - 1)
                printf("DHT20:      Retrying...\n");

            continue;
        }

        status = readStatus();

        // Re-attempt if not calibrated
        if (!(status & DHT20_STATUS_CALIBRATED)) {
            printf("DHT20:      Calibration error\n");
            goto retry;
        }

        printf("DHT20:      Initialised successfully; I2C status: 0x%x\n", status);

        this->initialised = true;
        return true;

    retry:
        // Delay and print retrying message if failed
        if (i < attempts - 1) {
            sleep_ms(2000);
            printf("DHT20:      Retrying...\n");
        }
    }

    printf("DHT20:      Failed to initialise\n");

    return false;
}

/// @brief Reads and updates sensor data
/// @return True on success, false on error
bool DHT20::updateData() {
    if (!initialised) return false;

    // Trigger read
    constexpr uint8_t triggerCmd[] = { DHT20_READ_CMD, 0x33, 0x00 };
    i2c_write_timeout_per_char_us(I2C_PORT, i2cAddress, triggerCmd, 3, false, I2C_PER_CHAR_TIMEOUT_US);

    // Wait for read
    if (!waitForProcessing()) {
        return false;
    }

    uint8_t readData[6];
    i2c_read_timeout_per_char_us(I2C_PORT, i2cAddress, readData, 6, false, I2C_PER_CHAR_TIMEOUT_US);

    // Decode humidity
    uint32_t humidityBuf = readData[1];
    humidityBuf <<= 8;
    humidityBuf |= readData[2];
    humidityBuf <<= 4;
    humidityBuf |= readData[3] >> 4;

    // Decode temperature
    uint32_t temperatureBuf = readData[3] & 0x0F;
    temperatureBuf <<= 8;
    temperatureBuf |= readData[4];
    temperatureBuf <<= 8;
    temperatureBuf |= readData[5];

    // Update data
    temperature = ((float)temperatureBuf * 200 / 0x100000) - 50;
    humidity = ((float)humidityBuf * 100) / 0x100000;

    auto absoluteTime = get_absolute_time();
    lastUpdated = to_ms_since_boot(absoluteTime);

    return true;
}

sensorData_t DHT20::getData() {
    return sensorData_t{
        .dht20 = dht20Data_t {
            .temperature = this->temperature,
            .humidity = this->humidity,
            .lastUpdated = this->lastUpdated
        }
    };
}

/// @brief Wait for DHT20 status busy pin to return to 0
/// @param useRTOSDelay Determines whether `vTaskDelay` or `sleep_ms` should be used - defaults to true
/// @return True on success, false if exceeded the I2C timeout
bool DHT20::waitForProcessing(const bool useRTOSDelay) {
    const uint16_t maxCount = DHT20_PROCESSING_TIMEOUT_MS / 10;
    uint16_t count = 0;

    while (readStatus() & DHT20_STATUS_BUSY) {
        if (useRTOSDelay) vTaskDelay(10);
        else sleep_ms(10);
        count += 10;

        if (count > maxCount) return false;
    }

    return true;
}
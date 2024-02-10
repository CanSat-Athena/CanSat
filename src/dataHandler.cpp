#include "dataHandler.h"
#include "globals.h"

void DataHandler::dataHandlerTask(void* DHPointer) {
    TickType_t lastStartTime;

    while (true) {
        // Get start time
        lastStartTime = xTaskGetTickCount();

        dataLine_t data{};

        // Update timestamp
        absolute_time_t absoluteTime = get_absolute_time();
        data.timestamp = to_ms_since_boot(absoluteTime);

        // Get DHT20 data
        for (int i = 0; i < DHT20_READ_FREQ; i++) {
            sensorData_t dhtData{};

            if (xQueueReceive(dataHandler->dht20Queue, &dhtData, (TickType_t)10) == pdPASS) {
                data.dht20[i] = dhtData.dht20;
            }
        }

        // Get BME680 data
        for (int i = 0; i < BME680_READ_FREQ; i++) {
            sensorData_t bmeData{};

            if (xQueueReceive(dataHandler->bme680Queue, &bmeData, (TickType_t)10) == pdPASS) {
                data.bme680[i] = bmeData.bme680;
            }
        }

        // Get light data
        for (int i = 0; i < LIGHT_READ_FREQ; i++) {
            sensorData_t lightData{};

            if (xQueueReceive(dataHandler->lightQueue, &lightData, (TickType_t)10) == pdPASS) {
                data.lightData[i] = lightData.lightData;
            }
        }

        // Write the data
        dataHandler->filesystem->addData(data);

        // Delay
        vTaskDelayUntil(&lastStartTime, 1000);
    }
}
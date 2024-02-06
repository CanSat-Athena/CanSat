#include "dataHandler.h"
#include "globals.h"

void DataHandler::dataHandlerTask(void* DHPointer) {
    TickType_t lastStartTime;

    vTaskDelay(500);

    while (true) {
        // Get start time
        lastStartTime = xTaskGetTickCount();

        dataLine_t data{};

        for (int i = 0; i < DHT20_READ_FREQ; i++) {
            sensorData_t dhtData;

            if (xQueueReceive(dataHandler->dht20Queue, &dhtData, (TickType_t)10) == pdPASS) {
                data.dht20[i] = dhtData.dht20;
            }
        }

        dataHandler->filesystem->addData(data);

        // Delay
        vTaskDelayUntil(&lastStartTime, 1000);
    }
}
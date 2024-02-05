#include "dataHandler.h"
#include "globals.h"

void DataHandler::dataHandlerTask(void* DHPointer) {
    TickType_t lastStartTime;

    vTaskDelay(500);

    while (true) {
        // Get start time
        lastStartTime = xTaskGetTickCount();

        dataLine_t data{};

        // for (int i = 0; i < DHT20_READ_FREQ; i++) {
        //     sensorData_t dhtData;

        //     if (xQueueReceive(dataHandler->dht20Queue, &dhtData, (TickType_t)10) == pdPASS) {
        //         data.dht20[i] = dhtData.dht20;
        //     }
        // }
        // lfs_file_t dataFile;
        // char dataFileName[100];
        // sprintf(dataFileName, "data_%u", dataHandler->filesystem->bootCount);
        // lfs_file_open(&(dataHandler->filesystem->lfs), &dataFile, dataFileName, LFS_O_WRONLY);

        // char a[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        // int err = lfs_file_write(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile), &a, strlen(a + 1));
        // lfs_file_close(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile));

        printf("%d\n", dataHandler->filesystem->addData(data));

        // Delay
        vTaskDelayUntil(&lastStartTime, 1000);
    }
}
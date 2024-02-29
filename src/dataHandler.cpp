#include "dataHandler.h"
#include "globals.h"
#include "streamHandler.h"

uint8_t dhtQueueStorageBuffer[DHT20_READ_FREQ * 2 * sizeof(dht20Data_t)];
uint8_t bmeQueueStorageBuffer[BME680_READ_FREQ * 2 * sizeof(bme680Data_t)];
uint8_t imuQueueStorageBuffer[IMU_READ_FREQ * 2 * sizeof(imuData_t)];
uint8_t lightQueueStorageBuffer[LIGHT_READ_FREQ * 2 * sizeof(lightData_t)];
uint8_t anemometerQueueStorageBuffer[ANEMOMETER_READ_FREQ * 2 * sizeof(anemometerData_t)];
uint8_t gpsQueueStorageBuffer[3 * 2 * sizeof(gpsData_t)];

StaticQueue_t dhtQueueBuffer;
StaticQueue_t bmeQueueBuffer;
StaticQueue_t imuQueueBuffer;
StaticQueue_t lightQueueBuffer;
StaticQueue_t anemometerQueueBuffer;
StaticQueue_t gpsQueueBuffer;

void DataHandler::dataHandlerTask(void* DHPointer) {
    // Wait for initialisation to complete
    xEventGroupWaitBits(eventGroup, 0b00000001, pdFALSE, pdTRUE, portMAX_DELAY);
    vTaskDelay(1000);

    // Get start time
    TickType_t lastStartTime = xTaskGetTickCount();

    while (true) {

        dataLine_t data{};
        dataRadioLine_t radioData{};

        // Update timestamp
        absolute_time_t absoluteTime = get_absolute_time();
        data.timestamp = to_ms_since_boot(absoluteTime);
        radioData.timestamp = to_ms_since_boot(absoluteTime);

        // Get DHT20 data
        for (int i = 0; i < DHT20_READ_FREQ; i++) {
            sensorData_t dhtData{};

            if (xQueueReceive(dataHandler->dht20Queue, &dhtData, (TickType_t)10) == pdPASS) {
                data.dht20[i] = dhtData.dht20;
                radioData.dht20[i] = dhtData.dht20;
            }
        }

        // Get BME680 data
        for (int i = 0; i < BME680_READ_FREQ; i++) {
            sensorData_t bmeData{};

            if (xQueueReceive(dataHandler->bme680Queue, &bmeData, (TickType_t)10) == pdPASS) {
                data.bme680[i] = bmeData.bme680;
                radioData.bme680[i] = bmeData.bme680;
            }
        }

        // Get IMU data
        for (int i = 0; i < IMU_READ_FREQ; i++) {
            sensorData_t imuData{};

            if (xQueueReceive(dataHandler->imuQueue, &imuData, (TickType_t)10) == pdPASS) {
                data.imu[i] = imuData.imu;
                radioData.imu[i] = imuData.imu;
            }
        }

        // Get light data
        for (int i = 0; i < LIGHT_READ_FREQ; i++) {
            sensorData_t lightData{};

            if (xQueueReceive(dataHandler->lightQueue, &lightData, (TickType_t)10) == pdPASS) {
                data.lightData[i] = lightData.lightData;
                radioData.lightData[i] = lightData.lightData;
            }
        }

        // Get wind data
        for (int i = 0; i < ANEMOMETER_READ_FREQ; i++) {
            sensorData_t anemometerData{};

            if (xQueueReceive(dataHandler->anemometerQueue, &anemometerData, (TickType_t)10) == pdPASS) {
                data.anemometerData[i] = anemometerData.anemometerData;
                radioData.anemometerData[i] = anemometerData.anemometerData;
            }
        }

        // Get GPS data
        sensorData_t gpsData = GPS::getDataStatic();
        data.gpsData[0] = gpsData.gpsData;
        radioData.gpsData[0] = gpsData.gpsData;

        // Write the data
        dataHandler->filesystem->addData(data);
        xQueueSendToBack(StreamHandler::dataQueue, &radioData, 100);

        // Delay
        vTaskDelayUntil(&lastStartTime, 1000);
    }
}
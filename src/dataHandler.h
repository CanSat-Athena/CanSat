#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <pico/stdlib.h>

#include "commonTypes.h"
#include "config.h"
#include "filesystem.hpp"

extern uint8_t dhtQueueStorageBuffer[DHT20_READ_FREQ * 2 * sizeof(dht20Data_t)];
extern uint8_t bmeQueueStorageBuffer[BME680_READ_FREQ * 2 * sizeof(bme680Data_t)];
extern uint8_t imuQueueStorageBuffer[IMU_READ_FREQ * 2 * sizeof(imuData_t)];
extern uint8_t lightQueueStorageBuffer[LIGHT_READ_FREQ * 2 * sizeof(lightData_t)];
extern uint8_t anemometerQueueStorageBuffer[ANEMOMETER_READ_FREQ * 2 * sizeof(anemometerData_t)];
extern uint8_t gpsQueueStorageBuffer[3 * 2 * sizeof(gpsData_t)];

extern StaticQueue_t dhtQueueBuffer;
extern StaticQueue_t bmeQueueBuffer;
extern StaticQueue_t imuQueueBuffer;
extern StaticQueue_t lightQueueBuffer;
extern StaticQueue_t anemometerQueueBuffer;
extern StaticQueue_t gpsQueueBuffer;

class DataHandler {
public:
QueueHandle_t dht20Queue, bme680Queue, imuQueue, lightQueue, gpsQueue, anemometerQueue;
    Filesystem* filesystem;

    DataHandler() {
        dht20Queue = xQueueCreateStatic(DHT20_READ_FREQ * 2, sizeof(dht20Data_t), dhtQueueStorageBuffer, &dhtQueueBuffer);
        bme680Queue = xQueueCreateStatic(BME680_READ_FREQ * 2, sizeof(bme680Data_t), bmeQueueStorageBuffer, &bmeQueueBuffer);
        imuQueue = xQueueCreateStatic(IMU_READ_FREQ * 2, sizeof(imuData_t), imuQueueStorageBuffer, &imuQueueBuffer);
        lightQueue = xQueueCreateStatic(LIGHT_READ_FREQ * 2, sizeof(lightData_t), lightQueueStorageBuffer, &lightQueueBuffer);
        anemometerQueue = xQueueCreateStatic(ANEMOMETER_READ_FREQ * 2, sizeof(anemometerData_t), anemometerQueueStorageBuffer, &anemometerQueueBuffer);
        gpsQueue = xQueueCreateStatic(3, sizeof(gpsData_t), gpsQueueStorageBuffer, &gpsQueueBuffer);

        // Set up filesystem
        filesystem = new Filesystem();
    }

    static void dataHandlerTask(void* DH_pointer);
};
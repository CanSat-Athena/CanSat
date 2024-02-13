#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <pico/stdlib.h>

#include "commonTypes.h"
#include "config.h"
#include "filesystem.hpp"

class DataHandler {
public:
    QueueHandle_t dht20Queue, bme680Queue, lightQueue, gpsQueue;
    Filesystem* filesystem;

    DataHandler() {
        dht20Queue = xQueueCreate(DHT20_READ_FREQ * 2, sizeof(dht20Data_t));
        bme680Queue = xQueueCreate(BME680_READ_FREQ * 2, sizeof(bme680Data_t));
        lightQueue = xQueueCreate(LIGHT_READ_FREQ * 2, sizeof(lightData_t));
        gpsQueue = xQueueCreate(3, sizeof(gpsData_t));

        // Set up filesystem
        filesystem = new Filesystem();
    }

    static void dataHandlerTask(void* DH_pointer);
};
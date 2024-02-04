#include <FreeRTOS.h>
#include <queue.h>
#include <pico/stdlib.h>

#include "commonTypes.h"
#include "config.h"
#include "filesystem.hpp"

class DataHandler {
protected:
public:
    QueueHandle_t dht20Queue, bme680Queue, lightQueue;
    Filesystem* filesystem;

    DataHandler() {
        dht20Queue = xQueueCreate(DHT20_READ_FREQ * 2, sizeof(dht20Data_t));
        bme680Queue = xQueueCreate(BME680_READ_FREQ * 2, sizeof(bme680Data_t));
        lightQueue = xQueueCreate(LIGHT_READ_FREQ * 2, sizeof(lightData_t));

        // Set up filesystem
        filesystem = new Filesystem();
    }

    static void dataHandlerTask(void* DH_pointer);
};
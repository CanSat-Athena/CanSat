#include <FreeRTOS.h>
#include <queue.h>
#include <pico/stdlib.h>

#include "commonTypes.h"
#include "config.h"

class DataHandler {
    protected:
    public:
    QueueHandle_t dht20Queue, bme680Queue, lightQueue;

    DataHandler() {
        dht20Queue = xQueueCreate(DHT20_READ_FREQ, sizeof(dht20Data_t));
        bme680Queue = xQueueCreate(BME680_READ_FREQ, sizeof(bme680Data_t));
        lightQueue = xQueueCreate(LIGHT_READ_FREQ, sizeof(lightData_t));
    }
};
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/exception.h>

#include "exceptionHandlers.h"
#include "config.h"
#include "dht20.h"
#include "bme680.h"
#include "i2c.h"

#define DEBUG_MODE

DHT20* dht;
BME680* bme;

/// @brief Setup sensors
void setup() {
    I2C::init();

    dht = new DHT20();
    bme = new BME680();

    puts("Setup complete\n");
}

void printTask(void* pvParameters) {
    while (true) {
        dht->updateData();
        bme->updateData();

        printf("DHT: temp: %f C, humidity %f %%\n", dht->temperature, dht->humidity);
        printf("BME680: last updated: %lu, temp: %.2f, pressure: %.2f, humidity: %.2f, g resistance: %.2f, status: 0x%x, g index: %d, m index: %d\n",
            bme->lastUpdated,
            bme->temperature,
            bme->pressure,
            bme->humidity,
            bme->gasResistance,
            bme->readStatus,
            bme->gasIndex,
            bme->measureIndex);
        // TODO: update to make sure argument is always at least 10
        vTaskDelay(500 - bme->timeTaken);
    }
}

int main() {
    stdio_init_all();

    // Safe hardfault handler
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    setup();

    TaskHandle_t printTaskHandle;

    xTaskCreate(printTask, "print", 512, NULL, 2, &printTaskHandle);

    vTaskStartScheduler();
    return 0;
}
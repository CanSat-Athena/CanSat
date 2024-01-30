#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/exception.h>

#include "exceptionHandlers.h"
#include "config.h"
#include "dht20.h"
#include "bme680.h"
#include "lightSensor.h"
#include "i2c.h"
#include "filesystem.hpp"
#include "dataHandler.h"
#include "commonTypes.h"

DHT20* dht;
BME680* bme;
LightSensor* light;
Filesystem* fs;
DataHandler* datahandler;

/// @brief Setup sensors
void setup() {
    printf("------------------\n");
    I2C::init();
    ADC::init();

    // Set up filesystem
    fs = new Filesystem();

    // Set up sensors
    dht = new DHT20();
    bme = new BME680();
    light = new LightSensor();

    // Set up data handler
    datahandler = new DataHandler();

    printf("Setup complete\n------------------\n");
}

void printTask(void* pvParameters) {
    while (true) {
        // dht->updateData();
        // bme->updateData();
        // light->updateData();

        // printf("DHT: temp: %f C, humidity %f %%\n", dht->temperature, dht->humidity);
        // printf("BME680: last updated: %lu, temp: %.2f, pressure: %.2f, humidity: %.2f, g resistance: %.2f, status: 0x%x, g index: %d, m index: %d\n",
        //     bme->lastUpdated,
        //     bme->temperature,
        //     bme->pressure,
        //     bme->humidity,
        //     bme->gasResistance,
        //     bme->readStatus,
        //     bme->gasIndex,
        //     bme->measureIndex);
        // printf("Light: ADC value: %d\n", light->lightValue);

        // TODO: update to make sure argument is always at least 10
        vTaskDelay(500 - bme->timeTaken);
    }
}

void sensorReadTask(void* pvParameters) {
    TickType_t lastStartTime;
    sensor_t* sensor = (sensor_t*)pvParameters;

    while (true) {
        // Get start time
        lastStartTime = xTaskGetTickCount();

        // Read data
        printf("Reading from %s\n", sensor->name);
        sensor->sensor->updateData();

        // Dump data to queue
        sensorData_t data = sensor->sensor->getData();
        xQueueSendToBack(*(sensor->queue), &data, 10);

        // Delay
        vTaskDelayUntil(&lastStartTime, DHT20_READ_TIME);
    }
}

int main() {
    stdio_init_all();

    // Safe hardfault handler
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    setup();

    TaskHandle_t printTaskHandle;
    xTaskCreate(printTask, "print", 512, NULL, 2, &printTaskHandle);

    sensor_t dht20 = {
        .sensor = dht,
        .name = (char*)"DHT20",
        .queue =  &(datahandler->dht20Queue),
        .updateFreq = DHT20_READ_FREQ,
        .updateTime = DHT20_READ_TIME
    };
    TaskHandle_t dht20ReadTask;
    xTaskCreate(sensorReadTask, "DHT20 read", 512, &dht20, 2, &dht20ReadTask);

    vTaskStartScheduler();
    return 0;
}
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/exception.h>

#include "exceptionHandlers.h"
#include "config.h"
#include "dht20.h"
#include "bme680.h"
#include "imu.h"
#include "gps.h"
#include "lightSensor.h"
#include "anemometer.h"
#include "i2c.h"
#include "dataHandler.h"
#include "commonTypes.h"
#include "globals.h"

DHT20* dht;
BME680* bme;
GPS* gps;
IMU* imu;
LightSensor* light;
Anemometer* anemometer;

/// @brief Setup sensors
void setup() {
    printf("------------------\n");
    I2C::init();
    ADC::init();
    
    // Set up sensors
    gps = new GPS();
    dht = new DHT20();
    bme = new BME680();
    imu = new IMU();
    light = new LightSensor();
    anemometer = new Anemometer();

    // Set up data handler
    dataHandler = new DataHandler();
    printf("Setup complete\n------------------\n");
    xEventGroupSetBits(eventGroup, 0b00000001);     // Set bit 0 to show initialisation complete
}

void sensorReadTask(void* pvParameters) {
    TickType_t lastStartTime = xTaskGetTickCount();
    sensor_t* sensor = (sensor_t*)pvParameters;

    while (true) {
        // Read data
        // printf("Reading from %s\n", sensor->name);
        sensor->sensor->updateData();

        // Dump data to queue
        sensorData_t data = sensor->sensor->getData();
        xQueueSendToBack(*(sensor->queue), &data, 10);

        // Delay
        vTaskDelayUntil(&lastStartTime, sensor->updateTime);
    }
}

void initTask(void* pvParameters) {
    setup();

    sensor_t dht20 = {
        .sensor = dht,
        .name = (char*)"DHT20",
        .queue = &(dataHandler->dht20Queue),
        .updateFreq = DHT20_READ_FREQ,
        .updateTime = DHT20_READ_TIME
    };
    TaskHandle_t dht20ReadTask;
    xTaskCreate(sensorReadTask, "DHT20 read", 512, &dht20, 2, &dht20ReadTask);

    sensor_t bme680 = {
        .sensor = bme,
        .name = (char*)"BME680",
        .queue = &(dataHandler->bme680Queue),
        .updateFreq = BME680_READ_FREQ,
        .updateTime = BME680_READ_TIME
    };
    TaskHandle_t bme680ReadTask;
    xTaskCreate(sensorReadTask, "BME680 read", 512, &bme680, 2, &bme680ReadTask);

    sensor_t imuSensor = {
        .sensor = imu,
        .name = (char*)"IMU",
        .queue = &(dataHandler->imuQueue),
        .updateFreq = IMU_READ_FREQ,
        .updateTime = IMU_READ_TIME
    };
    TaskHandle_t imuReadTask;
    xTaskCreate(sensorReadTask, "IMU read", 512, &imuSensor, 2, &imuReadTask);

    sensor_t lightSensor = {
        .sensor = light,
        .name = (char*)"Light",
        .queue = &(dataHandler->lightQueue),
        .updateFreq = LIGHT_READ_FREQ,
        .updateTime = LIGHT_READ_TIME
    };
    TaskHandle_t lightReadTask;
    xTaskCreate(sensorReadTask, "Light sensor read", 512, &lightSensor, 2, &lightReadTask);

    sensor_t anemometerSensor = {
        .sensor = anemometer,
        .name = (char*)"Anemometer",
        .queue = &(dataHandler->anemometerQueue),
        .updateFreq = ANEMOMETER_READ_FREQ,
        .updateTime = ANEMOMETER_READ_TIME
    };
    TaskHandle_t anemometerReadTask;
    xTaskCreate(sensorReadTask, "Anemometer sensor read", 512, &anemometerSensor, 2, &anemometerReadTask);

    TaskHandle_t dataHandlerTaskHandle;
    xTaskCreate(DataHandler::dataHandlerTask, "Data handler", 4096, NULL, 3, &dataHandlerTaskHandle);

    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();

    // Safe hardfault handler
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    eventGroup = xEventGroupCreate();
    xTaskCreate(initTask, "Init", 1024, NULL, 4, NULL);

    vTaskStartScheduler();
    return 0;
}
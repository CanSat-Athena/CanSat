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
#include "streamHandler.h"
#include "lightSensor.h"
#include "anemometer.h"
#include "i2c.h"
#include "dataHandler.h"
#include "commonTypes.h"
#include "globals.h"
#include "watchdog.h"

DHT20* dht;
BME680* bme;
GPS* gps;
IMU* imu;
LightSensor* light;
Anemometer* anemometer;

/// @brief Setup sensors
void setup() {
    printf("\n------------------\n");
    Watchdog::init();
    StreamHandler::init();
    StreamHandler::startLongPrint();
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
    StreamHandler::tPrintf("Setup complete\n------------------\n");
    StreamHandler::endLongPrint();
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
    TaskHandle_t dht20ReadTask = xTaskCreateStatic(sensorReadTask, "DHT20 read", DHT20_TASK_SIZE, &dht20, 2, dhtTaskStack, &dhtTaskBuffer);

    sensor_t bme680 = {
        .sensor = bme,
        .name = (char*)"BME680",
        .queue = &(dataHandler->bme680Queue),
        .updateFreq = BME680_READ_FREQ,
        .updateTime = BME680_READ_TIME
    };
    TaskHandle_t bme680ReadTask = xTaskCreateStatic(sensorReadTask, "BME680 read", BME680_TASK_SIZE, &bme680, 2, bmeTaskStack, &bmeTaskBuffer);

    sensor_t imuSensor = {
        .sensor = imu,
        .name = (char*)"IMU",
        .queue = &(dataHandler->imuQueue),
        .updateFreq = IMU_READ_FREQ,
        .updateTime = IMU_READ_TIME
    };
    TaskHandle_t imuReadTask;
    xTaskCreateStatic(sensorReadTask, "IMU read", IMU_TASK_SIZE, &imuSensor, 2, imuTaskStack, &imuTaskBuffer);

    sensor_t lightSensor = {
        .sensor = light,
        .name = (char*)"Light",
        .queue = &(dataHandler->lightQueue),
        .updateFreq = LIGHT_READ_FREQ,
        .updateTime = LIGHT_READ_TIME
    };
    TaskHandle_t lightReadTask;
    xTaskCreateStatic(sensorReadTask, "Light sensor read", LIGHT_TASK_SIZE, &lightSensor, 2, lightTaskStack, &lightTaskBuffer);

    sensor_t anemometerSensor = {
        .sensor = anemometer,
        .name = (char*)"Anemometer",
        .queue = &(dataHandler->anemometerQueue),
        .updateFreq = ANEMOMETER_READ_FREQ,
        .updateTime = ANEMOMETER_READ_TIME
    };
    TaskHandle_t anemometerReadTask;
    xTaskCreateStatic(sensorReadTask, "Anemometer sensor read", 512, &anemometerSensor, 2, anemometerTaskStack, &anemometerTaskBuffer);

    TaskHandle_t dataHandlerTaskHandle = xTaskCreateStatic(DataHandler::dataHandlerTask, "Data handler", DATA_HANDLER_TASK_SIZE, NULL, 3, dataHandlerTaskStack, &dataHandlerTaskBuffer);

    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();

    if (watchdog_caused_reboot()) {
        printf("\n\nReboot caused by watchdog!\n");
    }

    // Safe hardfault handler
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    eventGroup = xEventGroupCreateStatic(&eventGroupStack);
    xTaskCreateStatic(initTask, "Init", INIT_TASK_SIZE, NULL, 6, initTaskStack, &initTaskBuffer);

    vTaskStartScheduler();
    return 0;
}
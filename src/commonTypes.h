#pragma once
#include "pico/stdlib.h"
#include <FreeRTOS.h>
#include <queue.h>

#include "sensor.h"

typedef struct sensor_t {
    Sensor* sensor;

    // Readable name, used for print statements
    char* name;

    // FreeRTOS queue to dump data onto
    QueueHandle_t* queue;

    // Update intervals
    uint16_t updateFreq;
    uint16_t updateTime;
} sensor_t;


/*
   Sensor Data Structs
   Used for sharing data read from sensors
*/

typedef struct dht20Data_t {
    float temperature;
    float humidity;
    uint64_t lastUpdated;
} dht20Data_t;

typedef struct bme680Data_t {
    float temperature;
    float humidity;
    float pressure;
    float gasResistance;
    uint8_t gasIndex;
    uint8_t measureIndex;
    uint32_t timeTaken;
    uint8_t readStatus;
    uint8_t lastUpdated;
} bme680Data_t;

typedef struct lightData_t {
    uint16_t lightIntensity;
    uint64_t lastUpdated;
} lightData_t;
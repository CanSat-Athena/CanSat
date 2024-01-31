#pragma once
#include "pico/stdlib.h"
#include <FreeRTOS.h>
#include <queue.h>

// #include "sensor.h"

/*
   Sensor Data Structs
   Used for sharing data read from sensors
*/

typedef struct dht20Data_t {
    float temperature;
    float humidity;
    uint32_t lastUpdated;
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
    uint32_t lastUpdated;
} bme680Data_t;

typedef struct lightData_t {
    uint16_t lightIntensity;
    uint32_t lastUpdated;
} lightData_t;

typedef union sensorData_t {
    dht20Data_t dht20;
    bme680Data_t bme680;
    lightData_t lightData;
} sensorData_t;

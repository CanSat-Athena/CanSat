#pragma once
#include "pico/stdlib.h"
#include <FreeRTOS.h>
#include <queue.h>

#include "config.h"

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

/*
   Sensor Data Structs
   Used for sharing data read from sensors
*/

typedef struct dht20Data_t {
    float temperature;
    float humidity;
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
} bme680Data_t;

typedef struct lightData_t {
    uint16_t lightIntensity;
} lightData_t;

typedef union sensorData_t {
    dht20Data_t dht20;
    bme680Data_t bme680;
    lightData_t lightData;
} sensorData_t;

// Data to be stored
typedef struct dataLine_t {
    uint32_t timestamp;
    dht20Data_t dht20[(int)max(DHT20_READ_FREQ, 1)];
    bme680Data_t bme680[(int)max(BME680_READ_FREQ, 1)];
    lightData_t lightData[(int)max(LIGHT_READ_FREQ, 1)];
} dataLine_t;

/// @brief Filesystem action type (can be DATA_READ, DATA_WRITE)
enum fsActionType { DATA_READ, DATA_WRITE };

// Filesystem action
typedef struct fsAction_t {
    fsActionType type;
    union data {
        dataLine_t write;
    };
} fsAction_t;
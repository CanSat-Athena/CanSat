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
} __attribute__((packed)) dht20Data_t;

typedef struct bme680Data_t {
    float temperature;
    float humidity;
    float pressure;
    float gasResistance;
} __attribute__((packed)) bme680Data_t;

typedef struct imuData_t {
    int16_t accel[3],
        gyro[3],
        mag[3];
} __attribute__((packed)) imuData_t;

typedef struct lightData_t {
    uint16_t lightIntensity;
} __attribute__((packed)) lightData_t;

typedef struct anemometerData_t {
    uint32_t triggerCount;
} __attribute__((packed)) anemometerData_t;

typedef struct gpsData_t {
    double latitude;
    double longitude;
    float altitude;

    uint8_t fix;
} __attribute__((packed))  gpsData_t;

typedef union sensorData_t {
    dht20Data_t dht20;
    bme680Data_t bme680;
    imuData_t imu;
    lightData_t lightData;
    gpsData_t gpsData;
    anemometerData_t anemometerData;
} __attribute__((packed)) sensorData_t;

// Data to be stored
typedef struct dataLine_t {
    uint32_t timestamp;
    dht20Data_t dht20[(int)max(DHT20_READ_FREQ, 1)];
    bme680Data_t bme680[(int)max(BME680_READ_FREQ, 1)];
    imuData_t imu[(int)max(IMU_READ_FREQ, 1)];
    lightData_t lightData[(int)max(LIGHT_READ_FREQ, 1)];
    anemometerData_t anemometerData[(int)max(ANEMOMETER_READ_FREQ, 1)];
    gpsData_t gpsData[1];
} __attribute__((packed)) dataLine_t;

// Data to be sent
typedef struct dataRadioLine_t {
    uint32_t timestamp;
    dht20Data_t dht20[(int)max(DHT20_READ_FREQ, 1)];
    bme680Data_t bme680[(int)max(BME680_READ_FREQ, 1)];
    imuData_t imu[(int)max(IMU_READ_FREQ, 1)];
    lightData_t lightData[(int)max(LIGHT_READ_FREQ, 1)];
    anemometerData_t anemometerData[(int)max(ANEMOMETER_READ_FREQ, 1)];
    gpsData_t gpsData[1];
} __attribute__((packed)) dataRadioLine_t;

typedef struct packet_t {
    char type;
    uint8_t body[RADIO_MAX_PACKET_SIZE];
} __attribute__((packed)) packet_t;

#include <assert.h>
static_assert(sizeof(dataRadioLine_t) <= RADIO_MAX_PACKET_SIZE, "RADIO_MAX_PACKET_SIZE must be greater than size of dataRadioLine_t");
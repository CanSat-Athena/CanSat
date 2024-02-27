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
    // uint8_t gasIndex;
    // uint8_t measureIndex;
    // uint8_t readStatus;
} bme680Data_t;

typedef struct imuData_t {
    float accel_g[3],
    gyro_dps[3],
    mag_ut[3];
} imuData_t;

typedef struct lightData_t {
    uint16_t lightIntensity;
} lightData_t;

typedef struct anemometerData_t {
    uint32_t triggerCount;
} anemometerData_t;

typedef struct gpsData_t {
    double latitude;
    double longitude;
    double altitude;

    uint8_t fix;
} gpsData_t;

typedef union sensorData_t {
    dht20Data_t dht20;
    bme680Data_t bme680;
    imuData_t imu;
    lightData_t lightData;
    gpsData_t gpsData;
    anemometerData_t anemometerData;
} sensorData_t;

// Data to be stored
typedef struct dataLine_t {
    uint32_t timestamp;
    dht20Data_t dht20[(int)max(DHT20_READ_FREQ, 1)];
    bme680Data_t bme680[(int)max(BME680_READ_FREQ, 1)];
    imuData_t imu[(int)max(IMU_READ_FREQ, 1)];
    lightData_t lightData[(int)max(LIGHT_READ_FREQ, 1)];
    anemometerData_t anemometerData[(int)max(ANEMOMETER_READ_FREQ, 1)];
    gpsData_t gpsData[1];
} dataLine_t;

// Data to be sent
typedef struct dataRadioLine_t {
    uint32_t timestamp;
    dht20Data_t dht20[(int)max(DHT20_READ_FREQ, 1)];
    bme680Data_t bme680[(int)max(BME680_READ_FREQ, 1)];
    imuData_t imu[(int)max(IMU_READ_FREQ, 1)];
    lightData_t lightData[(int)max(LIGHT_READ_FREQ, 1)];
    anemometerData_t anemometerData[(int)max(ANEMOMETER_READ_FREQ, 1)];
    gpsData_t gpsData[1];
} dataRadioLine_t;

typedef struct packet_t {
    char type;
    uint8_t body[RADIO_MAX_PACKET_SIZE - 1];
} packet_t;

// /// @brief Filesystem action type (can be DATA_READ, DATA_WRITE)
// enum fsActionType { DATA_READ, DATA_WRITE };

// // Filesystem action
// typedef struct fsAction_t {
//     fsActionType type;
//     union data {
//         dataLine_t write;
//     };
// } fsAction_t;
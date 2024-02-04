#pragma once
#include <pico/stdlib.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include "config.h"
#include "commonTypes.h"

class Sensor {
protected:
    bool initialised = false;

public:
    uint32_t lastUpdated;

    virtual bool init(const uint attempts = 3) { return false; }
    virtual bool updateData() { return false; }
    virtual sensorData_t getData() { return sensorData_t{}; }

    /// @brief Checks if the sensor is initialised
    /// @return true if the sensor has been initialised
    bool isInitialised() {
        return initialised;
    }
};

typedef struct sensor_t {
    Sensor* sensor;

    // Readable name, used for print statements
    char* name;

    // FreeRTOS queue to dump data onto
    QueueHandle_t* queue;

    // Update intervals
    float updateFreq;
    uint16_t updateTime;
} sensor_t;
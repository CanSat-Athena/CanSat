#pragma once
#include "pico/stdlib.h"
#include <stdio.h>

#include "config.h"

class Sensor {
protected:
    bool initialised = false;

public:
    uint32_t lastUpdated;
    
    bool init(const uint attempts = 3) { return false; }
    bool updateData() { return false; }

    /// @brief Checks if the sensor is initialised
    /// @return true if the sensor has been initialised
    bool isInitialised() {
        return initialised;
    }
};
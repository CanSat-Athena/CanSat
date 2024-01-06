#include <pico/stdlib.h>
#include <stdio.h>

#include "sensor.h"
#include "config.h"
#include "adc.h"

#define LIGHT_SENSOR_GPIO 26

class LightSensor : Sensor {
protected:
public:
    uint16_t lightValue;
    
    LightSensor(bool initialise = true) {
        if (initialise) init();
    }
    
    bool init(const uint attempts = 3);
    bool updateData();
};

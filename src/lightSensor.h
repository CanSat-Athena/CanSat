#include <pico/stdlib.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>

#include "sensor.h"
#include "config.h"
#include "adc.h"
#include "globals.h"

#define LIGHT_SENSOR_GPIO 26

class LightSensor : public Sensor {
protected:
public:
    uint16_t lightValue;
    
    LightSensor(bool initialise = true) {
        if (initialise) init();
    }
    
    bool init(const uint attempts = 3);
    bool updateData();
};

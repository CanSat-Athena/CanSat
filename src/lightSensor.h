#include <pico/stdlib.h>
#include <stdio.h>

#include "sensor.h"
#include "config.h"
#include "adc.h"

class LightSensor : Sensor {
protected:
public:
    LightSensor(bool initialise = true) {
        if (initialise) init();
    }
    
    bool init(const uint attempts = 3);
};

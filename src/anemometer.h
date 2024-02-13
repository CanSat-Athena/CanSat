#include "hardware/gpio.h"

#include "config.h"
#include "globals.h"

class Anemometer : public Sensor {
private:
    static volatile uint32_t pulseCount;

    static void gpioInterruptHandler(uint gpio, uint32_t event_mask);

public:
    Anemometer(bool initialise = true) {
        if (initialise) init();
    }

    bool init(const uint attempts = 3);

    bool updateData() { return true; }
    sensorData_t getData();
};
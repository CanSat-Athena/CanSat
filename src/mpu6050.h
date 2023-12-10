#include "pico/stdlib.h"
#include <stdio.h>

#include "pins.h"
#include "i2c.h"

#define WHOAMI_REG 0x75

class MPU6050 {
private:
    bool initialised = false;

public:
    MPU6050(bool initialise = true);

    void init();

    bool isInitialised() {
        return initialised;
    }
};
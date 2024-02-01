#include "lightSensor.h"

/// @brief Initialise the light sensor
/// @param attempts Number of attempts to try before giving up 
/// @return True if succeeded
bool LightSensor::init(const uint attempts) {
    ADC::init();
    ADC::initGPIO(LIGHT_SENSOR_GPIO);
    this->initialised = true;
    printf("Light:      Initialised\n");

    return true;
}

/// @brief Updates the sensor data
/// @return True on success
bool LightSensor::updateData() {
    if (!initialised) return false;
    if (xSemaphoreTake(sensorReadMutex, (TickType_t)BME680_READ_TIME) != pdTRUE) return false;

    lightValue = ADC::readADC(LIGHT_SENSOR_GPIO);

    return true;
}
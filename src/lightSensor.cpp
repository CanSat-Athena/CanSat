#include "lightSensor.h"
#include "streamHandler.h"

/// @brief Initialise the light sensor
/// @param attempts Number of attempts to try before giving up 
/// @return True if succeeded
bool LightSensor::init(const uint attempts) {
    ADC::init();
    ADC::initGPIO(LIGHT_SENSOR_PIN);
    this->initialised = true;
    StreamHandler::tPrintf("Light:      Initialised\n");

    return true;
}

/// @brief Updates the sensor data
/// @return True on success
bool LightSensor::updateData() {
    if (!initialised) return false;
    lightValue = ADC::readADC(LIGHT_SENSOR_PIN);

    return true;
}

/// @brief Returns the data as a struct
/// @return The data
sensorData_t LightSensor::getData() {
    return {
       .lightData = lightData_t {
            .lightIntensity = lightValue
       } 
    };
}
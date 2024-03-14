#include "anemometer.h"
#include "streamHandler.h"

volatile uint32_t Anemometer::pulseCount = 0;

bool Anemometer::init(const uint attempts) {
    if (initialised) return false;

    // Initialise GPIO
    StreamHandler::tPrintf("Anemometer: Initialising GPIO\n");
    gpio_init(ANEMOMETER_PIN);
    gpio_set_dir(ANEMOMETER_PIN, GPIO_IN);
    gpio_pull_up(ANEMOMETER_PIN);

    // Set up interrupt
    gpio_set_irq_enabled_with_callback(ANEMOMETER_PIN, GPIO_IRQ_EDGE_FALL, true, gpioInterruptHandler);

    return true;
}

/// @brief GPIO interrupt handler for anemometer
/// @param gpio unused
/// @param event_mask unused
void Anemometer::gpioInterruptHandler(uint gpio, uint32_t event_mask) {
    pulseCount++;
}

sensorData_t Anemometer::getData() {
    uint32_t oldPulseCount = this->pulseCount;
    this->pulseCount = 0;

    return {
        .anemometerData = anemometerData_t {
            .triggerCount = oldPulseCount
        }
    };
}
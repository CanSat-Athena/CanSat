#include "watchdog.h"

/// @brief Initialise watchdog
void Watchdog::init() {
    watchdog_enable(WATCHDOG_TIME, true);

    // Create watchdog task
    xTaskCreate(watchdogTask, "Watchdog", 512, NULL, 30, NULL);
}

/// @brief Task to update watchdog
/// @param pvParameters unused
void Watchdog::watchdogTask(void *pvParameters) {
    TickType_t lastStartTime = xTaskGetTickCount();

    while (true) {
        watchdog_update();

        // Delay
        vTaskDelayUntil(&lastStartTime, WATCHDOG_TASK_TIME);
    }
}
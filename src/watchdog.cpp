#include "watchdog.h"

StackType_t watchdogStack[WATCHDOG_TASK_SIZE];
StaticTask_t watchdogTaskBuffer;

/// @brief Initialise watchdog
void Watchdog::init() {
    watchdog_enable(WATCHDOG_TIME, true);

    // Create watchdog task
    xTaskCreateStatic(watchdogTask, "Watchdog", WATCHDOG_TASK_SIZE, NULL, configMAX_PRIORITIES - 1, watchdogStack, &watchdogTaskBuffer);
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
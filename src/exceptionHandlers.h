// Used for resetting pico (no builtin reset functionality)
#include "hardware/watchdog.h"

#define log_core_num(fmt, ...) printf("[core %d] " fmt, portGET_CORE_ID(), ##__VA_ARGS__)

static void waitForDebugger() {
    log_core_num("Waiting for debugger...\n");
#ifndef DEBUG_MODE
    // Reset
    watchdog_enable(1, 1);
#endif
    __breakpoint();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    log_core_num("Stack overflow in task '%s'\n", pcTaskName);
#ifndef DEBUG_MODE
    // Reset
    watchdog_enable(1, 1);
#endif
    waitForDebugger();
}

static void hardfault_handler() {
    log_core_num("hardfault!!!\n");
#ifndef DEBUG_MODE
    // Reset
    watchdog_enable(1, 1);
#endif
    waitForDebugger();
}
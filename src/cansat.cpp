#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/exception.h"

#define log_core_num(fmt, ...) printf("[core %d] " fmt, portGET_CORE_ID(), ##__VA_ARGS__)

static void waitForDebugger() {
    log_core_num("Waiting for debugger...\n");
    __breakpoint();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    log_core_num("Stack overflow in task '%s'\n", pcTaskName);
    waitForDebugger();
}

static void hardfault_handler() {
    log_core_num("hardfault!!!\n");
    waitForDebugger();
}

void printTask(void* pvParameters) {
    puts("Hello world!");

    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();

    TaskHandle_t printTaskHandle;
    xTaskCreate(printTask, "print", 512, NULL, 2, &printTaskHandle);

    vTaskStartScheduler();
    return 0;
}
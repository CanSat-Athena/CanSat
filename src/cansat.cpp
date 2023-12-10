#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/exception.h"
#include "exceptionHandlers.h"

#include "pins.h"
#include "mpu6050.h"
#include "i2c.h"

#define DEBUG_MODE

MPU6050* mpu;

/// @brief Setup sensors
void setup() {
    I2C::init();
    mpu = new MPU6050();

    puts("Setup complete");
}

void printTask(void* pvParameters) {
    while (true) {
        puts("Hello world!");
        vTaskDelay(1000);
    }
}

int main() {
    stdio_init_all();

    // Safe hardfault handler
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    setup();

    TaskHandle_t printTaskHandle;

    xTaskCreate(printTask, "print", 512, NULL, 2, &printTaskHandle);

    vTaskStartScheduler();
    return 0;
}
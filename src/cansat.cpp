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

void setup() {
    // Setup I2C properly
    gpio_init(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_init(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    // Don't forget the pull ups! | Or use external ones
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Init I2C
    i2c_init(I2C_PORT, 100 * 1000); // 100kHz
}

void printTask(void* pvParameters) {
    puts("Hello world!");

    vTaskDelete(NULL);
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
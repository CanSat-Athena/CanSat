#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/exception.h"
#include "exceptionHandlers.h"

#include "pins.h"
#include "dht20.h"
#include "i2c.h"

#define DEBUG_MODE

DHT20* dht;

/// @brief Setup sensors
void setup() {
    I2C::init();
    
    dht = new DHT20();

    puts("Setup complete");
}

void printTask(void* pvParameters) {
    while (true) {
        puts("Hello world!");
        dht->updateData();

        printf("The temperature is: %f C, humidity %f %%\n", dht->temperature, dht->humidity);
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
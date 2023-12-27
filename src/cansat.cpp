#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"
#include "hardware/exception.h"
#include "exceptionHandlers.h"

#include "pins.h"
#include "dht20.h"
#include "bme680.h"
#include "i2c.h"

#define DEBUG_MODE

DHT20* dht;
BME680* bme;

/// @brief Setup sensors
void setup() {
    I2C::init();
    
    dht = new DHT20();
    bme = new BME680();

    puts("Setup complete\n");
}

void printTask(void* pvParameters) {
    while (true) {
    //     dht->updateData();

    //     printf("The temperature is: %f C, humidity %f %%\n", dht->temperature, dht->humidity);
    //     vTaskDelay(500);
        tight_loop_contents();
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
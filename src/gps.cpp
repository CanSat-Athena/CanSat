#include "gps.h"

QueueHandle_t GPS::gpsQueue;
volatile char GPS::gpsLine[100];
volatile uint8_t GPS::gpsLineIndex;
lwgps_t GPS::lwgps;

bool GPS::init() {
    if (initialised) return false;

    // Initialise UART
    printf("GPS:        Initialising UART\n");
    uart_init(GPS_UART, 9600);
    gpio_set_function(GPS_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPS_RX_PIN, GPIO_FUNC_UART);

    // Set data format
    uart_set_format(GPS_UART, GPS_DATA_BITS, GPS_STOP_BITS, GPS_PARITY);

    // Turn off FIFOs
    uart_set_fifo_enabled(GPS_UART, false);

    // Initialise lwgps
    printf("GPS:        Initialising lwgps\n");
    lwgps_init(&lwgps);

    // Set up buffer
    printf("GPS:        Setting up buffer\n");
    gpsQueue = xQueueCreate(10, sizeof(char[100]));

    // Set up task
    printf("GPS:        Setting up task\n");
    xTaskCreate(gpsTask, "GPS read", 512, NULL, 2, NULL);

    printf("GPS:        Initialised\n");
    return true;
}

void GPS::gpsTask(void* pvParameters) {
    char line[100];
    uint8_t lineIndex = 0;

    xEventGroupWaitBits(eventGroup, 0b00000001, pdFALSE, pdTRUE, portMAX_DELAY);   // Wait for initialisation to complete
    vTaskDelay(100);

    // Set up and enable interrupt handlers
    int UART_IRQ = GPS_UART == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, uartInterruptHandler);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(GPS_UART, true, false);

    while (true) {
        // Wait for data
        if (xQueueReceive(gpsQueue, &line, portMAX_DELAY) == 1) {
            if (strStartsWith(line, "$GPGGA") || strStartsWith(line, "$GNGGA") ||
                strStartsWith(line, "$GPGSA") || strStartsWith(line, "$GNGSA") ||
                strStartsWith(line, "$GPGSV") || strStartsWith(line, "$GNGSV") ||
                strStartsWith(line, "$GPRMC") || strStartsWith(line, "$GNRMC")) {
                // Process GPS data
                uint8_t err = lwgps_process(&lwgps, line, strlen(line));
            }
        }
    }
}

void GPS::uartInterruptHandler() {
    while (uart_is_readable(GPS_UART)) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        char character = uart_getc(GPS_UART);

        // Add character to line
        gpsLine[gpsLineIndex++] = character;

        if (character == '\n' || gpsLineIndex >= 99) {
            gpsLine[gpsLineIndex] = '\0';
            xQueueSendToBackFromISR(gpsQueue, (void*)&gpsLine, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            gpsLineIndex = 0;
        }
    }
}

sensorData_t GPS::getDataStatic() {
    return {
        .gpsData = gpsData_t {
            .latitude = lwgps.latitude,
            .longitude = lwgps.longitude,
            .altitude = lwgps.altitude,

            .fix = lwgps.fix
        }
    };
}
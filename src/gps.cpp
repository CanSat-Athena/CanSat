#include "gps.h"

StreamBufferHandle_t GPS::gpsBuffer;
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
    gpsBuffer = xStreamBufferCreate(256, 1);

    // Set up task
    printf("GPS:        Setting up task\n");
    xTaskCreate(gpsTask, "GPS read", 512, NULL, 2, NULL);

    printf("GPS:        Initialised\n");
    return true;
}

void GPS::gpsTask(void* pvParameters) {
    uint32_t ulNotificationValue;
    char singleCharBuffer;
    char line[100];
    uint8_t lineIndex = 0;

    vTaskDelay(2000);   // TODO: Remove this delay

    // Set up and enable interrupt handlers
    int UART_IRQ = GPS_UART == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, uartInterruptHandler);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(GPS_UART, true, false);

    while (true) {
        // Wait for data
        if (xStreamBufferReceive(gpsBuffer, &singleCharBuffer, 1, portMAX_DELAY) == 1) {
            line[lineIndex++] = singleCharBuffer;

            if (singleCharBuffer == '\n' || lineIndex >= 99) {
                line[lineIndex] = '\0';

                // Process GPS data
                uint8_t err = lwgps_process(&lwgps, line, strlen(line));

                printf("GPS:        %d, %d\n", lwgps.is_valid, lwgps.seconds);
                lineIndex = 0;
            }
        }
    }
}

void GPS::uartInterruptHandler() {
    while (uart_is_readable(GPS_UART)) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        char character = uart_getc(GPS_UART);

        xStreamBufferSendFromISR(gpsBuffer, &character, 1, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
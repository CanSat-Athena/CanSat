#include "gps.h"
#include "streamHandler.h"

QueueHandle_t GPS::gpsQueue;
volatile char GPS::gpsLine[100];
volatile uint8_t GPS::gpsLineIndex;
lwgps_t GPS::lwgps;

// GPS buffer queues
uint8_t gpsBufferQueueStorageBuffer[GPS_QUEUE_SIZE];
StaticQueue_t gpsBufferQueueBuffer;

/// @brief Initialise GPS
bool GPS::init() {
    if (initialised) return false;

    // Initialise UART
    StreamHandler::tPrintf("GPS:        Initialising UART\n");
    uart_init(GPS_UART, 9600);
    gpio_set_function(GPS_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPS_RX_PIN, GPIO_FUNC_UART);

    // Set data format
    uart_set_format(GPS_UART, GPS_DATA_BITS, GPS_STOP_BITS, GPS_PARITY);

    // Turn off FIFOs
    uart_set_fifo_enabled(GPS_UART, false);

    // Initialise lwgps
    StreamHandler::tPrintf("GPS:        Initialising lwgps\n");
    lwgps_init(&lwgps);

    // Set up buffer
    StreamHandler::tPrintf("GPS:        Setting up buffer\n");
    gpsQueue = xQueueCreateStatic(GPS_QUEUE_SIZE, sizeof(char[100]), gpsBufferQueueStorageBuffer, &gpsBufferQueueBuffer);

    // Set up task
    StreamHandler::tPrintf("GPS:        Setting up task\n");
    xTaskCreateStatic(gpsTask, "GPS read", GPS_TASK_SIZE, NULL, 2, gpsTaskStack, &gpsTaskBuffer);

    StreamHandler::tPrintf("GPS:        Initialised\n");
    return true;
}

void GPS::gpsTask(void* pvParameters) {
    char line[100];
    uint8_t lineIndex = 0;

    // Wait for initialisation to complete
    xEventGroupWaitBits(eventGroup, 0b00000001, pdFALSE, pdTRUE, portMAX_DELAY);
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
            .altitude = (float)lwgps.altitude,

            .fix = lwgps.fix
        }
    };
}
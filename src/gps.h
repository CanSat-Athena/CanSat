#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <stream_buffer.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/irq.h>
#include "lwgps/lwgps.h"

#include "sensor.h"
#include "globals.h"
#include "commonTypes.h"
#include "config.h"

#define GPS_BAUD_RATE 115200
#define GPS_DATA_BITS 8
#define GPS_STOP_BITS 1
#define GPS_PARITY    UART_PARITY_NONE

class GPS : public Sensor {
private:
    static const UBaseType_t gpsTaskNotificationArrayIndex = 1;
    static StreamBufferHandle_t gpsBuffer;

    static lwgps_t lwgps;

public:

    GPS(bool initialise = true) {
        if (initialise) init();
    }

    bool init();

    static void gpsTask(void* pvParameters);
    static void uartInterruptHandler();
};
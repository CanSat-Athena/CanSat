#pragma once
#include <pico/stdio.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <stream_buffer.h>
#include <queue.h>
#include <task.h>

#include "radio.hpp"
#include "commonTypes.h"
#include "config.h"

class StreamHandler {
public:
    static inline QueueHandle_t dataQueue;
    static inline StreamBufferHandle_t terminalBuffer;

    static void init();

    static void terminalBufferTask(void *unused);
    static void dataQueueTask(void *unused);

    static void tPrintf(const char *string, ...);
};
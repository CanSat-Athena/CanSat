#pragma once
#include <pico/stdio.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <stream_buffer.h>
#include <queue.h>
#include <timers.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>

#include "radio.hpp"
#include "commonTypes.h"
#include "config.h"

class StreamHandler {
private:
    static inline StreamBufferHandle_t terminalBuffer;
    static inline StreamBufferHandle_t inputBuffer;

    static inline TimerHandle_t inputTimer;
    static inline StaticTimer_t inputTimerBuffer;

    static inline SemaphoreHandle_t printSemaphore;

    static inline Radio* radio;

public:
    static inline QueueHandle_t dataQueue;

    static void init();

    static void terminalBufferTask(void* unused);
    static void dataQueueTask(void* unused);

    static void inputTimerCallback(TimerHandle_t xTimer);

    static char getChar();
    static void tPrintf(const char* string, ...);
    static void rPrintf(const char* string, ...);

    static void startLongPrint();
    static void endLongPrint();
};

#include <cstdio>
#include <cstring>
#include <algorithm>

#include "streamHandler.h"
#include "globals.h"

// Task stacks
static StackType_t terminalBufferStack[TERMINAL_BUFFER_TASK_SIZE];
static StackType_t dataQueueStack[DATA_QUEUE_TASK_SIZE];
static StackType_t radioStack[RADIO_TASK_SIZE];

// Task buffers
static StaticTask_t terminalBufferTaskBuffer;
static StaticTask_t dataQueueTaskBuffer;
static StaticTask_t radioTaskBuffer;

// Data queue
static uint8_t dataQueueStorageBuffer[DATA_QUEUE_SIZE * sizeof(dataRadioLine_t)];
static StaticQueue_t dataQueueBuffer;

// Terminal stream buffer
static uint8_t terminalStreamBufferStorageArea[TERMINAL_BUFFER_SIZE + 1];
static StaticStreamBuffer_t terminalStaticStreamBuffer;

// Input buffer
static uint8_t inputStreamBufferStorageArea[INPUT_BUFFER_SIZE + 1];
static StaticStreamBuffer_t inputStaticStreamBuffer;

// Mutex buffer
static StaticSemaphore_t printSemaphoreBuffer;

/// @brief Initialise the stream handler
void StreamHandler::init() {
    // Create queues + buffers
    printf("StreamHnd:  Initialising queues and buffers\n");
    dataQueue = xQueueCreateStatic(DATA_QUEUE_SIZE, RADIO_MAX_PACKET_SIZE, dataQueueStorageBuffer, &dataQueueBuffer);
    terminalBuffer = xStreamBufferCreateStatic(TERMINAL_BUFFER_SIZE, 1, terminalStreamBufferStorageArea, &terminalStaticStreamBuffer);
    inputBuffer = xStreamBufferCreateStatic(INPUT_BUFFER_SIZE, 1, inputStreamBufferStorageArea, &inputStaticStreamBuffer);
    printSemaphore = xSemaphoreCreateMutexStatic(&printSemaphoreBuffer);

    // Initialise radio
    radio = new Radio(inputBuffer);

    // Initialise timer
    printf("StreamHnd:  Initialising timer\n");
    inputTimer = xTimerCreateStatic("Input timer", INPUT_TIMER_PERIOD_MS, pdTRUE, (void*)0, inputTimerCallback, &inputTimerBuffer);

    // Create tasks
    printf("StreamHnd:  Creating tasks\n");
    xTaskCreateStatic(terminalBufferTask, "Terminal buffer", TERMINAL_BUFFER_TASK_SIZE, NULL, 3, terminalBufferStack, &terminalBufferTaskBuffer);
    xTaskCreateStatic(dataQueueTask, "Data queue", DATA_QUEUE_TASK_SIZE, NULL, 3, dataQueueStack, &dataQueueTaskBuffer);

    printf("StreamHnd:  Initialised\n");

    // startLongPrint();
}

/// @brief Task to handle the terminal buffer
void StreamHandler::terminalBufferTask(void* unused) {
    xTimerStart(inputTimer, 0);

    packet_t packet;
    packet.type = 't';

    uint32_t bytesRead = 0;

    while (true) {
        bytesRead = xStreamBufferReceive(terminalBuffer, &(packet.body), sizeof(packet.body) / sizeof(packet.body[0]) - 1, portMAX_DELAY);
        if (bytesRead > 0) {
            packet.body[bytesRead] = '\0';
            packet.size = bytesRead + 1;        // + 1 to account for \0
            if (Radio::initialised)
                xQueueSendToBack(Radio::radioQueue, &packet, portMAX_DELAY);
        }
    }
}

/// @brief Task to handle the data queue
void StreamHandler::dataQueueTask(void* unused) {
    // Wait for initialisation to complete
    xEventGroupWaitBits(eventGroup, 0b00000001, pdFALSE, pdTRUE, portMAX_DELAY);

    packet_t packet;
    packet.type = 'd';
    packet.size = std::min(sizeof(dataRadioLine_t), (size_t)RADIO_MAX_PACKET_SIZE);

    dataRadioLine_t data;

    while (true) {
        if (xQueueReceive(StreamHandler::dataQueue, &packet.body, portMAX_DELAY) == pdTRUE) {
            xQueueSendToBack(Radio::radioQueue, &packet, portMAX_DELAY);
        }
    }
}

/// @brief Input timer callback - handles input from stdin queue
/// @param t Repeating timer struct
/// @return true
void StreamHandler::inputTimerCallback(TimerHandle_t xTimer) {
    char character;
    int c;

    while (true) {
        c = getchar_timeout_us(0);

        if (c != PICO_ERROR_TIMEOUT) {
            character = (c & 0xFF);
            xStreamBufferSend(inputBuffer, &character, 1, 0);
        } else {
            break;
        }
    }
}

/// @brief Get a char from the receive input buffer
/// @return The character
char StreamHandler::getChar() {
    char received;
    xStreamBufferReceive(inputBuffer, &received, 1, portMAX_DELAY);
    return received;
}

/// @brief Prints to the terminal - sent over both UART and radio
/// @param string The string to print
void StreamHandler::tPrintf(const char* string, ...) {
    va_list args;
    va_start(args, string);     // Very important

    char buffer[512];
    vsnprintf(buffer, 512 - 1, string, args);  // -1 just to be safe
    buffer[512 - 1] = '\0';                    // Prevent memory leak, just in case

    printf("%s", buffer);

    xSemaphoreTake(printSemaphore, 20);
    size_t size = strlen(buffer);
    uint32_t offset = xStreamBufferSend(terminalBuffer, buffer, size, portMAX_DELAY);
    xSemaphoreGive(printSemaphore);

    va_end(args);
}

/// @brief Prints to the terminal - sent over radio only
/// @param string The string to print
void StreamHandler::rPrintf(const char* string, ...) {
    va_list args;
    va_start(args, string);     // Very important

    char buffer[512];
    vsnprintf(buffer, 512 - 1, string, args);  // -1 just to be safe
    buffer[512 - 1] = '\0';                    // Prevent memory leak, just in case

    xSemaphoreTake(printSemaphore, 20);
    size_t size = strlen(buffer);
    uint32_t offset = xStreamBufferSend(terminalBuffer, buffer, size, portMAX_DELAY);
    xSemaphoreGive(printSemaphore);

    va_end(args);
}

void StreamHandler::startLongPrint() {
    xStreamBufferSetTriggerLevel(terminalBuffer, RADIO_MAX_PACKET_SIZE);
}

void StreamHandler::endLongPrint() {
    xStreamBufferSetTriggerLevel(terminalBuffer, 1);
}
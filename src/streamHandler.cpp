#include <cstdio>
#include <cstring>
#include "streamHandler.h"
#include "globals.h"

#define DATA_QUEUE_SIZE 5
#define TERMINAL_BUFFER_SIZE RADIO_MAX_PACKET_SIZE * 6
#define INPUT_BUFFER_SIZE 256

// Queue stacks
static StackType_t terminalBufferStack[TERMINAL_BUFFER_TASK_SIZE];
static StackType_t dataQueueStack[DATA_QUEUE_TASK_SIZE];

// Task buffers
static StaticTask_t terminalBufferTaskBuffer;
static StaticTask_t dataQueueTaskBuffer;

// Data queue
static uint8_t dataQueueStorageBuffer[DATA_QUEUE_SIZE * sizeof(dataRadioLine_t)];
static StaticQueue_t dataQueueBuffer;

// Terminal stream buffer
static uint8_t terminalStreamBufferStorageArea[TERMINAL_BUFFER_SIZE + 1];
static StaticStreamBuffer_t terminalStaticStreamBuffer;

// Input buffer
static uint8_t inputStreamBufferStorageArea[INPUT_BUFFER_SIZE + 1];
static StaticStreamBuffer_t inputStaticStreamBuffer;

/// @brief Initialise the stream handler
void StreamHandler::init() {
    radio = Radio();

    printf("StreamHnd:  Initialising queues and buffers\n");
    dataQueue = xQueueCreateStatic(DATA_QUEUE_SIZE, sizeof(dataRadioLine_t), dataQueueStorageBuffer, &dataQueueBuffer);
    terminalBuffer = xStreamBufferCreateStatic(TERMINAL_BUFFER_SIZE, 1, terminalStreamBufferStorageArea, &terminalStaticStreamBuffer);
    inputBuffer = xStreamBufferCreateStatic(INPUT_BUFFER_SIZE, 1, inputStreamBufferStorageArea, &inputStaticStreamBuffer);

    printf("StreamHnd:  Initialising timer\n");
    inputTimer = xTimerCreateStatic("Input timer", INPUT_TIMER_PERIOD_MS, pdTRUE, (void*)0, inputTimerCallback, &inputTimerBuffer);

    printf("StreamHnd:  Creating tasks\n");
    xTaskCreateStatic(terminalBufferTask, "Terminal buffer", TERMINAL_BUFFER_TASK_SIZE, NULL, 3, terminalBufferStack, &terminalBufferTaskBuffer);
    xTaskCreateStatic(dataQueueTask, "Data queue", DATA_QUEUE_TASK_SIZE, NULL, 3, dataQueueStack, &dataQueueTaskBuffer);

    printf("StreamHnd:  Initialised\n");
}

/// @brief Task to handle the terminal buffer
void StreamHandler::terminalBufferTask(void* unused) {
    // Wait for initialisation to complete
    xEventGroupWaitBits(eventGroup, 0b00000001, pdFALSE, pdTRUE, portMAX_DELAY);
    xTimerStart(inputTimer, 0);

    packet_t packet;
    packet.type = 't';

    uint32_t bytesRead = 0;

    while (true) {
        bytesRead = xStreamBufferReceive(terminalBuffer, &(packet.body), (sizeof(packet.body) / sizeof(packet.body[0])) - 1, portMAX_DELAY);
        if (bytesRead > 0) {
            packet.body[bytesRead] = '\0';
            printf("%s", packet.body);
            LoRa.beginPacket();
            LoRa.print((const char*)packet.body, bytesRead + 1);
            LoRa.endPacket();
        }
    }
}

/// @brief Task to handle the data queue
void StreamHandler::dataQueueTask(void* unused) {
    packet_t packet;
    packet.type = 'd';

    dataRadioLine_t data;

    while (true) {
        if (xQueueReceive(StreamHandler::dataQueue, &data, portMAX_DELAY)) {
            // TODO: Do stuff, send over radio
        }
        vTaskDelay(300);
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

char StreamHandler::getChar() {
    char received;
    xStreamBufferReceive(inputBuffer, &received, 1, portMAX_DELAY);
    return received;
}

void StreamHandler::tPrintf(const char* string, ...) {
    va_list args;
    va_start(args, string);     // Very important

    static char buffer[TERMINAL_BUFFER_SIZE];
    vsnprintf(buffer, TERMINAL_BUFFER_SIZE - 1, string, args);  // -1 just to be safe
    buffer[TERMINAL_BUFFER_SIZE - 1] = '\0';                    // Prevent memory leak, just in case

    uint32_t offset = xStreamBufferSend(terminalBuffer, buffer, strlen(buffer), 30);

    va_end(args);
}
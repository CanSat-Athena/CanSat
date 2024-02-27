#include <cstdio>
#include "streamHandler.h"

#define DATA_QUEUE_SIZE 5
#define TERMINAL_BUFFER_SIZE RADIO_MAX_PACKET_SIZE * 4

StackType_t terminalBufferStack[TERMINAL_BUFFER_TASK_SIZE];
StackType_t dataQueueStack[DATA_QUEUE_TASK_SIZE];

StaticTask_t terminalBufferTaskBuffer;
StaticTask_t dataQueueTaskBuffer;

uint8_t dataQueueStorageBuffer[DATA_QUEUE_SIZE];
StaticQueue_t dataQueueBuffer;

uint8_t terminalStreamBufferStorageArea[TERMINAL_BUFFER_SIZE + 1];
StaticStreamBuffer_t terminalStaticStreamBuffer;

/// @brief Initialise the stream handler
void StreamHandler::init() {
    dataQueue = xQueueCreateStatic(DATA_QUEUE_SIZE, sizeof(dataRadioLine_t), dataQueueStorageBuffer, &dataQueueBuffer);
    terminalBuffer = xStreamBufferCreateStatic(RADIO_MAX_PACKET_SIZE * 4, 1, terminalStreamBufferStorageArea, &terminalStaticStreamBuffer);

    xTaskCreateStatic(terminalBufferTask, "Terminal buffer", TERMINAL_BUFFER_TASK_SIZE, NULL, 3, terminalBufferStack, &terminalBufferTaskBuffer);
}

/// @brief Task to handle the terminal buffer
void StreamHandler::terminalBufferTask(void* unused) {
    packet_t packet;
    packet.type = 'd';

    uint32_t bytesRead = 0;

    while (true) {
        bytesRead = xStreamBufferReceive(terminalBuffer, &(packet.body), (sizeof(packet.body) / sizeof(packet.body[0])) - 1, portMAX_DELAY);
        if (bytesRead > 0) {
            packet.body[bytesRead] = '\0';
            printf("%s", packet.body);
        }
    }
}

void StreamHandler::tPrintf(char *string, TickType_t timeout) {
    
}
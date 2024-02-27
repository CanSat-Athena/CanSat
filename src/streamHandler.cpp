#include <cstdio>

#include "streamHandler.h"

/// @brief Initialise the stream handler
void StreamHandler::init() {
    dataQueue = xQueueCreate(5, sizeof(dataRadioLine_t));
    terminalBuffer = xStreamBufferCreate(RADIO_MAX_PACKET_SIZE * 4, 1);

    xTaskCreate(terminalBufferTask, "Terminal buffer", 512, NULL, 3, NULL);
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
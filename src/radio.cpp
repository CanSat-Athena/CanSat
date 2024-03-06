#include "radio.hpp"

// Task stack & buffer
static StackType_t radioStack[RADIO_TASK_SIZE];
static StaticTask_t radioTaskBuffer;

// Radio queue
static uint8_t radioQueueStorageBuffer[RADIO_QUEUE_SIZE * sizeof(packet_t)];
static StaticQueue_t radioQueueBuffer;

/// @brief Initialise radio
void Radio::init() {
    // Set pin configuration
    LoRa.setPins(RADIO_NSS_PIN, RADIO_RESET_PIN, RADIO_DIO0_PIN);

    // Initialise queue
    radioQueue = xQueueCreateStatic(RADIO_QUEUE_SIZE, sizeof(packet_t), radioQueueStorageBuffer, &radioQueueBuffer);

    const uint attempts = 3;

    for (int i = 0; i < attempts; i++) {
        printf("Radio:      Trying to connect, attempt %d of %d\n", i + 1, attempts);

        int result = LoRa.begin(433000000);

        // Re-attempt if failed
        if (result != 1) {
            printf("Radio:      Failed to connect\n");
            sleep_ms(2000);

            if (i < attempts - 1)
                printf("Radio:      Retrying...\n");
            else break;

            continue;
        }

        printf("Radio:      Initialised successfully\n");
        LoRa.enableCrc();
        LoRa.setSignalBandwidth(RADIO_BANDWIDTH);
        LoRa.setSpreadingFactor(RADIO_SPREAD_FACTOR);

        xTaskCreateStatic(radioTask, "Radio", RADIO_TASK_SIZE, NULL, 6, radioStack, &radioTaskBuffer);
        return;
    }

    // Failed
    printf("Radio:      Failed to initialise\n");
}

/// @brief Send the packet
/// @param packet The packet to send
void Radio::send(packet_t packet) {
    LoRa.beginPacket();
    for (int i = 0; i < strlen((const char*)&packet); i++) {
        LoRa.write(((char*)&packet)[i]);
    }
    LoRa.endPacket();
}

/// @brief Interrupt service routine for when a packet is received
/// @param packetSize Size of the packet received
void Radio::receiveIsr(int packetSize) {
    printf("\nReceived!\n");
    // Read packet
    for (int i = 0; i < packetSize; i++) {
        char c = LoRa.read();
        printf("%c", c);
    }
}

/// @brief Radio task - sends items in radio queue
/// @param unused 
void Radio::radioTask(void* unused) {
    LoRa.onReceive(receiveIsr);
    LoRa.receive();

    packet_t packet;

    while (true) {
        if (xQueueReceive(radioQueue, &packet, portMAX_DELAY) == pdTRUE) {
            // printf("\n------------- Packet body:\n%s\nend\n", packet.body);
            send(packet);
            LoRa.receive();
        }
    }
}
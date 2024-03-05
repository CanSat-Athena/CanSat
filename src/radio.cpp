#include "radio.hpp"

/// @brief Initialise radio
void Radio::init() {
    const uint attempts = 3;

    LoRa.setPins(RADIO_NSS_PIN, RADIO_RESET_PIN, RADIO_DIO0_PIN);

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

        LoRa.onReceive(receiveIsr);
        LoRa.receive();

        return;
    }

    // Failed
    printf("Radio:      Failed to initialise\n");
}

/// @brief Send the packet
/// @param packet The packet to send
void Radio::send(packet_t& packet) {
    while (LoRa.beginPacket() == 0) {
        vTaskDelay(50);
    }

    LoRa.beginPacket();
    for (int i = 0; i < strlen((const char*)&packet); i++) {
        LoRa.write(((char*)&packet)[i]);
    }
    LoRa.endPacket(true);
}

void Radio::receiveIsr(int packetSize) {
    printf("\nReceived!\n");
    // Read packet
    for (int i = 0; i < packetSize; i++) {
        char c = LoRa.read();
        if (i > 0)
            printf("%c", c);
    }
}
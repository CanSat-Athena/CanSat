#pragma once
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include "pico-lora/src/LoRa-RP2040.h"

#include "config.h"
#include "commonTypes.h"


class Radio {
public:
    Radio() {
        init();
    }

    void init();
    void send(packet_t& packet);

    static void receiveIsr(int packetSize);
};
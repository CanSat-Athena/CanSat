#pragma once
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h>
#include "pico-lora/src/LoRa-RP2040.h"

#include "config.h"
#include "commonTypes.h"

class Radio {
private:
    static inline StreamBufferHandle_t inputBuffer;

public:
    static inline bool initialised = false;
    static inline QueueHandle_t radioQueue;

    Radio(StreamBufferHandle_t inputBuffer) {
        Radio::inputBuffer = inputBuffer;
        init();
    }

    void init();
    static void send(packet_t packet);

    static void receiveIsr(int packetSize);
    static void radioTask(void* unused);
};
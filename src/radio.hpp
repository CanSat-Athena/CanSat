#pragma once
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include "pico-lora/src/LoRa-RP2040.h"

class Radio {
public:
    Radio() {
        init();
    }
    
    void init();
};
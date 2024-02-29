#pragma once
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>

class Radio {
public:
    Radio() {
        init();
    }
    
    void init();
};
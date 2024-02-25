#pragma once
#include <pico/stdio.h>
#include <hardware/watchdog.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

class Watchdog {
public:
    static void init();
    static void watchdogTask(void *pvParameters);
};
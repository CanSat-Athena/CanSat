#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>
#include "dataHandler.h"

extern DataHandler* dataHandler;
extern EventGroupHandle_t eventGroup;

int32_t getIntInput();
bool strStartsWith(const char* a, const char* b);
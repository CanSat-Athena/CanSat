#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include "dataHandler.h"

extern DataHandler* dataHandler;

int32_t getIntInput();
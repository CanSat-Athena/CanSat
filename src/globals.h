#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <event_groups.h>
#include "dataHandler.h"
#include "gps.h"

extern DataHandler* dataHandler;
extern EventGroupHandle_t eventGroup;

// Event group stack
extern StaticEventGroup_t eventGroupStack;

// Task buffers (not all)
extern StackType_t initTaskStack[INIT_TASK_SIZE];
extern StackType_t dataHandlerTaskStack[DATA_HANDLER_TASK_SIZE];

extern StackType_t dhtTaskStack[DHT20_TASK_SIZE];
extern StackType_t bmeTaskStack[BME680_TASK_SIZE];
extern StackType_t imuTaskStack[IMU_TASK_SIZE];
extern StackType_t lightTaskStack[LIGHT_TASK_SIZE];
extern StackType_t anemometerTaskStack[ANEMOMETER_TASK_SIZE];
extern StackType_t gpsTaskStack[GPS_TASK_SIZE];

// Task buffers (not all)
extern StaticTask_t initTaskBuffer;
extern StaticTask_t dataHandlerTaskBuffer;

extern StaticTask_t dhtTaskBuffer;
extern StaticTask_t bmeTaskBuffer;
extern StaticTask_t imuTaskBuffer;
extern StaticTask_t lightTaskBuffer;
extern StaticTask_t anemometerTaskBuffer;
extern StaticTask_t gpsTaskBuffer;

int32_t getIntInput();
bool strStartsWith(const char* a, const char* b);
uint16_t getBatteryLevel();
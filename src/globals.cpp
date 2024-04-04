#include <algorithm>

#include "globals.h"
#include "streamHandler.h"
#include "adc.h"

DataHandler* dataHandler;
EventGroupHandle_t eventGroup;

// Event group stack
StaticEventGroup_t eventGroupStack;

// Task buffers (not all)
StackType_t initTaskStack[INIT_TASK_SIZE];
StackType_t dataHandlerTaskStack[DATA_HANDLER_TASK_SIZE];

StackType_t dhtTaskStack[DHT20_TASK_SIZE];
StackType_t bmeTaskStack[BME680_TASK_SIZE];
StackType_t imuTaskStack[IMU_TASK_SIZE];
StackType_t lightTaskStack[LIGHT_TASK_SIZE];
StackType_t anemometerTaskStack[ANEMOMETER_TASK_SIZE];
StackType_t gpsTaskStack[GPS_TASK_SIZE];

// Task buffers (not all)
StaticTask_t initTaskBuffer;
StaticTask_t dataHandlerTaskBuffer;

StaticTask_t dhtTaskBuffer;
StaticTask_t bmeTaskBuffer;
StaticTask_t imuTaskBuffer;
StaticTask_t lightTaskBuffer;
StaticTask_t anemometerTaskBuffer;
StaticTask_t gpsTaskBuffer;

// Gets an integer number from serial
int32_t getIntInput() {
    char characters[10]{};
    char c;

    for (int i = 0; i < sizeof(characters) / sizeof(char); i++) {
        // Break to avoid overflow
        if (i + 1 == sizeof(characters) / sizeof(char)) {
            characters[i] = '\0';
            StreamHandler::tPrintf("\n");
            break;
        }

        // Read char
        c = StreamHandler::getChar();

        // Handle enter key
        if (c == '\r' || c == '\n') {
            StreamHandler::tPrintf("\n");
            characters[i] = '\0';
            break;
        }

        // Handle backspace
        if (c == '\b') {
            if (i <= 0) {
                StreamHandler::tPrintf("\a");
                i -= 1;
            } else {
                StreamHandler::tPrintf("\b \b");
                i -= 2;
            }
            continue;
        }

        characters[i] = c;
        StreamHandler::tPrintf("%c", c);
    }

    return atoi(characters);
}

uint16_t getBatteryLevel() {
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    // const float conversionFactor = 3 * 3.3f / (1 << 12);
    uint16_t value = ADC::readADC(BATTERY_ADC_PIN, true);
    // float voltage = value * conversionFactor;
    // float percentage = 100 * ((voltage - BATTERY_EMPTY_VOLTAGE) / (BATTERY_FULL_VOLTAGE - BATTERY_EMPTY_VOLTAGE));
    // printf("%d, %f, %f\n", value, voltage, percentage);

    return value;
}

/// @brief Compares two strings to see if the start is same
/// @param a The string to be checked
/// @param b The substring
/// @return True if is a substring
bool strStartsWith(const char* a, const char* b) {
    return strncmp(a, b, strlen(b)) == 0;
}


/*  ---  FROM https://github.com/aws/amazon-freertos/blob/a126b0c55795be5986f86d4f6ef73bc5ed091c29/demos/demo_runner/aws_demo.c  ---  */

/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer,
    uint32_t* pulIdleTaskStackSize) {
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetPassiveIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetPassiveIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer,
    uint32_t* pulIdleTaskStackSize,
    BaseType_t xPassiveIdleTaskIndex) {
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
    StackType_t** ppxTimerTaskStackBuffer,
    uint32_t* pulTimerTaskStackSize) {
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/
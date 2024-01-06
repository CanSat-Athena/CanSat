#pragma once
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define ADC_PIN_0 26

/// @brief Wrapper around pico hardware/adc.h library
class ADC {
public:
    /// @brief Initialises ADC
    /// @return True on success
    static bool init() {        
        adc_init();
        return true;
    }

    /// @brief Initialises GPIO pin
    /// @param GPIO GPIO pin to initialise
    /// @return true on success
    static bool initGPIO(const uint GPIO) {
        adc_gpio_init(GPIO);
        return true;
    }

    /// @brief Selects GPIO pin for reading
    /// @param GPIO GPIO pin to select
    /// @return true on success
    static bool selectInput(const uint GPIO) {
        adc_select_input((uint)((int)GPIO - ADC_PIN_0));
        return true;
    }

    /// @brief Reads GPIO pin
    /// @param GPIO GPIO pin to read
    /// @param selectInput Runs selectInput() if true. Defaults to true
    /// @return The ADC value read
    static uint16_t readADC(const uint GPIO, const bool selectInput = true) {
        if (selectInput) ADC::selectInput(GPIO);
        return adc_read();
    }
};
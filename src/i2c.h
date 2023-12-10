#include "pico/stdlib.h"
#include "hardware/i2c.h"

void initI2C(int sdaPin = PICO_DEFAULT_I2C_SDA_PIN, int sclPin = PICO_DEFAULT_I2C_SCL_PIN) {
    // Setup I2C properly
    gpio_init(sdaPin);
    gpio_init(sclPin);
    gpio_set_function(sdaPin, GPIO_FUNC_I2C);
    gpio_set_function(sclPin, GPIO_FUNC_I2C);
    // Don't forget the pull ups! | Or use external ones
    gpio_pull_up(sdaPin);
    gpio_pull_up(sclPin);
}
#include "app_gpio.h"

void appGpioToggleTask(void * arg) {
    AppState_t* state = (AppState_t*) arg;
    
    state->gpio_relay_state_enum = APP_RELAY_STATE_ON;
    gpio_set_level(APP_GPIO_RELAY_OUTPUT_PIN, 1);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    state->gpio_relay_state_enum = APP_RELAY_STATE_OFF;
    gpio_set_level(APP_GPIO_RELAY_OUTPUT_PIN, 0);
    
    vTaskDelete(NULL);
}


void appGpioPinsInit(AppState_t *app_state) {
    gpio_config_t pin_config = {};
    pin_config.intr_type = GPIO_INTR_DISABLE;
    pin_config.mode = GPIO_MODE_OUTPUT;
    pin_config.pin_bit_mask = APP_OUTPUT_PIN_SEL;
    pin_config.pull_down_en = 1;
    pin_config.pull_up_en = 0;
    gpio_config(&pin_config);


    // Set initial levels //
    gpio_set_level(APP_GPIO_RELAY_OUTPUT_PIN, 0);
}
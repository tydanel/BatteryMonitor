#ifndef __APP_GPIO_H__
#define __APP_GPIO_H__

#include <driver/gpio.h>
#include "app.h"


#define APP_OUTPUT_PIN_SEL ( \
    (1ULL << GPIO_NUM_25) \
|   (1ULL << GPIO_NUM_26) \
|   (1ULL << GPIO_NUM_27) \
)


#define APP_INPUT_PIN_SEL (1ULL)
#define APP_GPIO_RELAY_OUTPUT_PIN GPIO_NUM_27

void appGpioToggleTask(void * arg);
void appGpioPinsInit(AppState_t *app_state);


#endif
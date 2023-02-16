#ifndef __APP_ADC_H__
#define __APP_ADC_H__


#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "app.h"

#define ADC_DEFAULT_VREF 1100
#define ADC_NUM_SAMPLES 64

void appAdcReadTask(void* args);
void appAdcPinsInit(AppState_t* app_state);

#endif
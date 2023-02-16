#include "app_adc.h"

static const char* TAG = "app_adc";
static esp_adc_cal_characteristics_t *adc_chars = NULL;

void appAdcReadTask(void* args) {
    AppState_t* app_state = (AppState_t*) args;

    for ( ;; ) {

        uint32_t voltage_in_reading  = 0;
        uint32_t current_in_reading  = 0;
        uint32_t voltage_out_reading = 0;
        uint32_t current_out_reading = 0;

        xEventGroupWaitBits(app_state->application_loop_tick_trigger,
            APP_ADC_READ_TASK_LOOP_TRIGGER_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY
        );
        xEventGroupClearBits(app_state->application_loop_tick_trigger,
            APP_ADC_READ_TASK_LOOP_TRIGGER_BIT);

        for ( int i = 0; i < ADC_NUM_SAMPLES; i++ ) {
            voltage_in_reading  += adc1_get_raw(ADC1_CHANNEL_4);
            current_in_reading  += adc1_get_raw(ADC1_CHANNEL_5);
            voltage_out_reading += adc1_get_raw(ADC1_CHANNEL_6);
            current_out_reading += adc1_get_raw(ADC1_CHANNEL_7);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        voltage_in_reading  /= ADC_NUM_SAMPLES;
        current_in_reading  /= ADC_NUM_SAMPLES;
        voltage_out_reading /= ADC_NUM_SAMPLES;
        current_out_reading /= ADC_NUM_SAMPLES;

        app_state->adc_voltage_in  = esp_adc_cal_raw_to_voltage(voltage_in_reading, adc_chars);
        app_state->adc_current_in  = esp_adc_cal_raw_to_voltage(current_in_reading, adc_chars);
        app_state->adc_voltage_out = esp_adc_cal_raw_to_voltage(voltage_out_reading, adc_chars);
        app_state->adc_current_out = esp_adc_cal_raw_to_voltage(current_out_reading, adc_chars);

        xEventGroupSetBits(app_state->application_loop_tick_trigger,
            APP_HTTP_CLIENT_TASK_LOOP_TRIGGER_BIT);
        
    }
}

void appAdcPinsInit(AppState_t* app_state) {
    if ( esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK ) {
        ESP_LOGI(TAG, "eFuse Two Point: Supported");
    } else {
        ESP_LOGI(TAG, "eFuse Two Point: NOT Supported");
    }

    if ( esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_DEFAULT_VREF) == ESP_OK ) {
        ESP_LOGI(TAG, "eFuse Vref: Supported");
    } else {
        ESP_LOGI(TAG, "eFuse Vref: NOT Supported");
    }

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11));


    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
        ADC_UNIT_1,
        ADC_ATTEN_DB_11,
        ADC_WIDTH_BIT_12,
        ADC_DEFAULT_VREF,
        adc_chars
    );


    if ( val_type == ESP_ADC_CAL_VAL_EFUSE_TP ) {
        ESP_LOGI(TAG, "Characterized using Two Point Value");
    } else if ( val_type == ESP_ADC_CAL_VAL_EFUSE_VREF ) {
        ESP_LOGI(TAG, "Characterized using eFuse Vref");
    } else {
        ESP_LOGI(TAG, "Characterized using Default Vref");
    }
}
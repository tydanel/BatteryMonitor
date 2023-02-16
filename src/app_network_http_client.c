#include "app_network_http_client.h"

static const char *TAG = "APP HTTP CLIENT";

char* create_post_data(AppState_t * state) {
    char * buffer = NULL;
    const char* template = 
    "{"
        "\"type\": \"STATISTICS\","
        "\"ip\": \"%s\","
        "\"current_in\": %f,"
        "\"voltage_in\": %f,"
        "\"current_out\": %f,"
        "\"voltage_out\": %f"
    "}";
    size_t needed = snprintf(NULL, 0, template,
        state->net_ip_address,
        state->adc_current_in,
        state->adc_voltage_in,
        state->adc_current_out,
        state->adc_voltage_out
    ) + 1;
    buffer = (char*)malloc(needed);
    sprintf(buffer, template, 
        state->net_ip_address,
        state->adc_current_in,
        state->adc_voltage_in,
        state->adc_current_out,
        state->adc_voltage_out
    );
    return buffer;
}

void appNetworkHttpClientTask(void* arg) {
    AppState_t* app_state = (AppState_t*) arg;

    for (;;) {
        xEventGroupSetBits(app_state->application_loop_tick_trigger,
            APP_ADC_READ_TASK_LOOP_TRIGGER_BIT);

        xEventGroupWaitBits(app_state->application_loop_tick_trigger,
            APP_HTTP_CLIENT_TASK_LOOP_TRIGGER_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
        xEventGroupClearBits(app_state->application_loop_tick_trigger,
            APP_HTTP_CLIENT_TASK_LOOP_TRIGGER_BIT);
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        

        char response_buffer[APP_HTTP_MAX_RX_BUFFER] = {0};

        esp_http_client_config_t config = {
            .host = "10.20.0.40",
            .port = 8000,
            .path = "/monitor",
            .user_data = response_buffer,
            .disable_auto_redirect = true
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);

        char* post_data = create_post_data(app_state);

        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, post_data, strlen(post_data));

        esp_err_t err = esp_http_client_perform(client);
        if ( err != ESP_OK ) {
            ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        }

        esp_http_client_close(client);
        esp_http_client_cleanup(client);

        free(post_data);
    }
}
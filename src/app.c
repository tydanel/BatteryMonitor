#include "app.h"

void appInitNvsFlash() {
    esp_err_t ret = nvs_flash_init();
    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void appEventHandleTask(void * arg) {
    const char* TAG = "appEventHandleTask";
    AppState_t* app_state = (AppState_t*) arg;
    for ( ;; ) {
        EventBits_t bits = xEventGroupWaitBits(
            app_state->application_event_group
        ,   APP_NET_WIFI_CONNECT_BIT 
        |   APP_NET_WIFI_FAIL_BIT
        |   APP_GPIO_RELAY_TRIGGER_BIT
        ,   pdFALSE
        ,   pdFALSE
        ,   portMAX_DELAY
        );
        
        if ( bits & APP_NET_WIFI_CONNECT_BIT ) {
            // WiFi Connected
            xEventGroupClearBits(
                app_state->application_event_group,
                APP_NET_WIFI_CONNECT_BIT
            );
            ESP_LOGI(TAG, "WiFi Connected.");
        }


        if ( bits & APP_NET_WIFI_FAIL_BIT ) {
            // WiFi Failed
            xEventGroupClearBits(
                app_state->application_event_group,
                APP_NET_WIFI_FAIL_BIT
            );
            ESP_LOGW(TAG, "WiFi Disconnected.");
        }

        if ( bits & APP_GPIO_RELAY_TRIGGER_BIT ) {
            // Initiate toggle relay
            xEventGroupClearBits(
                app_state->application_event_group,
                APP_GPIO_RELAY_TRIGGER_BIT
            );
            app_state->gpio_relay_state_enum = APP_RELAY_STATE_INIT;

            // Trigger the main thread loop so the new task is created to toggle the Relay
            ESP_LOGI(TAG, "Relay Toggle initiated.");
        }

        // Trigger main loop tick after any events //
        xEventGroupSetBits(app_state->application_loop_tick_trigger, APP_MAIN_TASK_LOOP_TRIGGER_BIT);
    }
}

AppState_t* appInitState() {
    AppState_t *app_state = (AppState_t*) calloc(1, sizeof(AppState_t));

    app_state->application_event_group       = xEventGroupCreate();
    app_state->application_loop_tick_trigger = xEventGroupCreate();

    app_state->adc_current_in  = 0.0;
    app_state->adc_voltage_in  = 0.0;
    app_state->adc_current_out = 0.0;
    app_state->adc_voltage_out = 0.0;

    app_state->gpio_relay_state = 0b0;
    app_state->gpio_relay_state_enum = APP_RELAY_STATE_OFF;
    app_state->gpio_relay_timer = 0;

    app_state->net_wifi_connected = false;
    app_state->net_ip_address = NULL;
    app_state->net_wifi_connect_retries = 0;
    app_state->net_wifi_event_group = xEventGroupCreate();

    return app_state;
}

void appDeintState(AppState_t* app_state) {
    vEventGroupDelete(app_state->application_event_group);
    vEventGroupDelete(app_state->application_loop_tick_trigger);
    vEventGroupDelete(app_state->net_wifi_event_group);

    free(app_state->net_ip_address);
    free(app_state);
}


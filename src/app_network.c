#include "app_network.h"

static const char* TAG = "APP NETWORK";

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data ) {
    AppState_t* app_state = (AppState_t *) arg;

    if ( event_base == WIFI_EVENT ) {
        if ( event_id == WIFI_EVENT_STA_START ) {
            ESP_ERROR_CHECK(esp_wifi_connect());
        } else if ( event_id == WIFI_EVENT_STA_DISCONNECTED ) {
            ESP_ERROR_CHECK(esp_wifi_connect());
            app_state->net_wifi_connected = false;
            app_state->net_wifi_connect_retries++;
            ESP_LOGI(TAG, "Retry wifi connect: %d", app_state->net_wifi_connect_retries);
        }
    } 
    if ( event_base == IP_EVENT ) {
        if ( event_id == IP_EVENT_STA_GOT_IP ) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;

            if ( app_state->net_ip_address != NULL ) {
                free(app_state->net_ip_address);
                app_state->net_ip_address = NULL;
            }

            size_t needed = snprintf(NULL, 0, IPSTR, IP2STR(&event->ip_info.ip)) ;
            app_state->net_ip_address = calloc(1, needed);
            sprintf(app_state->net_ip_address, IPSTR, IP2STR(&event->ip_info.ip));

            ESP_LOGI(TAG, "Got ip address: %s", app_state->net_ip_address);
            app_state->net_wifi_connect_retries = 0;
            app_state->net_wifi_connected = true;

            xEventGroupSetBits(app_state->net_wifi_event_group, WIFI_CONNECT_BIT);
            xEventGroupSetBits(app_state->application_event_group, APP_NET_WIFI_CONNECT_BIT);
        }
    }

}

void wifi_up_task(void * arg) {
    AppState_t* app_state = (AppState_t*) arg;

    for (;;) {
        EventBits_t bits = xEventGroupWaitBits(app_state->net_wifi_event_group,
            WIFI_CONNECT_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
        if ( bits & WIFI_CONNECT_BIT ) {
            ESP_LOGI(TAG, "Connected to ap SSID:%s password:%s",
                WIFI_SSID, WIFI_PASSWORD);
            xEventGroupClearBits(app_state->net_wifi_event_group, WIFI_CONNECT_BIT);
        } else if ( bits & WIFI_FAIL_BIT ) {
            ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                WIFI_SSID);
            xEventGroupClearBits(app_state->net_wifi_event_group, WIFI_FAIL_BIT);
        } else {
            ESP_LOGE(TAG, "UNEXPECTED ERROR");
            xEventGroupClearBits(app_state->net_wifi_event_group, bits);
        }
    }
}

void appNetworkInitWifiStation(AppState_t* app_state) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        wifi_event_handler,
        (void *) app_state,
        &instance_any_id
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        wifi_event_handler,
        (void *) app_state,
        &instance_got_ip
    ));


    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());


    esp_err_t ret = tcpip_adapter_set_hostname(
        TCPIP_ADAPTER_IF_STA,
        WIFI_HOSTANME
    );

    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG, "Failed to set hostname: %d", ret);
        ESP_ERROR_CHECK(ret);
    }

    ESP_LOGI(TAG, "WiFi init complete.");

    xTaskCreate(
        wifi_up_task,
        "WIFI_UP_TASK",
        4096,
        (void *) app_state,
        tskIDLE_PRIORITY,
        NULL
    );
}


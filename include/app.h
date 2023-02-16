#ifndef __APP_H__
#define __APP_H__

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_err.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include <esp_log.h>

#define true 1
#define false 0

// Application Event Handler event bits //
#define APP_GPIO_RELAY_TRIGGER_BIT  BIT0
#define APP_NET_WIFI_CONNECT_BIT    BIT12
#define APP_NET_WIFI_FAIL_BIT       BIT13
#define APP_ALL_EVENT_BITS


// Application task loop triggers       //
#define APP_MAIN_TASK_LOOP_TRIGGER_BIT          BIT0
#define APP_HTTP_CLIENT_TASK_LOOP_TRIGGER_BIT   BIT1
#define APP_ADC_READ_TASK_LOOP_TRIGGER_BIT      BIT2

typedef enum AppRelayState {
    APP_RELAY_STATE_INIT,
    APP_RELAY_STATE_OFF,
    APP_RELAY_STATE_ON,
} AppRelayState_t;

typedef struct AppState {
    EventGroupHandle_t application_event_group;
    EventGroupHandle_t application_loop_tick_trigger;

    // ================================= //
    // =========== ADC state =========== //
    // ================================= //
    float adc_voltage_in;
    float adc_current_in;
    float adc_voltage_out;
    float adc_current_out;

    // ================================= //
    // ========== GPIO state =========== //
    // ================================= //
    EventGroupHandle_t gpio_relay_event_group;
    AppRelayState_t gpio_relay_state_enum;
    uint8_t gpio_relay_state;
    uint16_t gpio_relay_timer;

    // ================================= //
    // ========= Network state ========= //
    // ================================= //
    char*               net_ip_address;
    unsigned int        net_wifi_connected;
    uint8_t             net_wifi_ssid[32];
    uint8_t             net_wifi_password[64];
    unsigned int        net_wifi_connect_retries;
    EventGroupHandle_t  net_wifi_event_group;
} AppState_t;

AppState_t* appInitState();
void appEventHandleTask(void * arg);
void appInitNvsFlash();
void appDeintState(AppState_t*);

#endif
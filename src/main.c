#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <sdkconfig.h>

#include "app.h"
#include "app_network.h"
#include "app_network_http_client.h"
#include "app_network_http_server.h"
#include "app_adc.h"
#include "app_gpio.h"


void app_main() {
    AppState_t * state = appInitState();
    
    appInitNvsFlash();
    
    // Create the event handler task first.
    xTaskCreate(
        appEventHandleTask,
        "EVENT_HANDLER",
        4096,
        (void *) state,
        tskIDLE_PRIORITY,
        NULL
    );
    

    // Initialize networking tasks  //
    appNetworkInitWifiStation( state );
    appNetworkInitHttpServer ( state );
    // appNetworkInitHttpClient (state);

    // Initialize IO pins           //
    appGpioPinsInit          ( state );
    appAdcPinsInit           ( state );

    xTaskCreate(
        appAdcReadTask,
        "ADC_READ_TASK",
        4096,
        (void *) state,
        tskIDLE_PRIORITY,
        NULL
    );

    xTaskCreate(
        appNetworkHttpClientTask,
        "HTTP_CLIENT_TASK",
        4096,
        (void *) state,
        tskIDLE_PRIORITY,
        NULL
    );

    // TaskHandle_t gpio_toggle_task_handle;

    while ( true ) {

        // EventBits_t bits = xEventGroupWaitBits(state->application_mainloop_trigger,
        xEventGroupWaitBits(state->application_loop_tick_trigger,
            APP_MAIN_TASK_LOOP_TRIGGER_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
        xEventGroupClearBits(state->application_loop_tick_trigger,
            APP_MAIN_TASK_LOOP_TRIGGER_BIT);

        if ( state->gpio_relay_state_enum == APP_RELAY_STATE_INIT ) {
            // xTask
            xTaskCreate(
                appGpioToggleTask,
                "GPIO_TOGGLE_TASK",
                2048,
                (void *) state,
                tskIDLE_PRIORITY,
                NULL
            );
        }
        
        // vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
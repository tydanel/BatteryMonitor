#ifndef __APP_NETWORK_H__
#define __APP_NETWORK_H__

#include <lwip/err.h>
#include <lwip/sys.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>

#include "app.h"

#define WIFI_CONNECT_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define WIFI_SSID "MikroTik-AP1"
#define WIFI_PASSWORD ""
#define WIFI_HOSTANME "DirkMinerMonitor01"



void appNetworkInitWifiStation(AppState_t* app_state);


#endif
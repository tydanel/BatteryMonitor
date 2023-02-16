#ifndef __APP_NETWORK_HTTP_CLIENT_H__
#define __APP_NETWORK_HTTP_CLIENT_H__

#include <string.h>
#include <esp_netif.h>
#include <esp_http_client.h>
#include "app.h"

#define APP_HTTP_MAX_TX_BUFFER (2048)
#define APP_HTTP_MAX_RX_BUFFER (512)



void appNetworkHttpClientTask(void* arg);


#endif
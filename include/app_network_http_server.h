#ifndef __APP_NETWORK_HTTP_SERVER_H__
#define __APP_NETWORK_HTTP_SERVER_H__

#include <esp_http_server.h>
#include "app.h"

httpd_handle_t appNetworkInitHttpServer(AppState_t* app_state);

#endif
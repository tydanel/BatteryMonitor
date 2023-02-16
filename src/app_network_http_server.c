#include "app_network_http_server.h"

esp_err_t post_handler(httpd_req_t *req) {

    char* content = NULL;
    
    AppState_t* app_state = (AppState_t*) req->user_ctx;

    printf("\nIP:%s\n", app_state->net_ip_address);
    // AppState_t* app_state = *app_state;

    size_t recv_size = req->content_len;
    content = malloc(recv_size + 1);
    int ret = httpd_req_recv(req, content, recv_size);
    content[recv_size] = '\0';

    if ( ret <= 0 ) {
        if ( ret == HTTPD_SOCK_ERR_TIMEOUT ) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if ( strcmp(content, "restart-miner") == 0 ) {
        printf("\nTime to restart the miner\n");
        // app_state->gpio_relay_state_enum = APP_RELAY_STATE_START_OFF;
        if ( app_state->gpio_relay_state_enum == APP_RELAY_STATE_OFF ) {
            xEventGroupSetBits(app_state->application_event_group, APP_GPIO_RELAY_TRIGGER_BIT);
            const char resp[] = "Toggling relay.";
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            goto free_and_return;
        } else {
            const char resp[] = "Toggle already in progress.";
            httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
            goto free_and_return;
        }
    }

    const char resp[] = "Unknown command.";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

free_and_return:
    free(content);
    return ESP_OK;
}

httpd_handle_t appNetworkInitHttpServer(AppState_t* app_state) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // config.global_user_ctx = app_state;


    httpd_handle_t server = NULL;

    httpd_uri_t uri_post = {
        .uri = "/",
        .method = HTTP_POST,
        .handler = post_handler,
        .user_ctx = app_state,
    };

    if ( httpd_start(&server, &config) == ESP_OK ) {
        httpd_register_uri_handler(server, &uri_post);
    }

    return server;
}

#pragma once

#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#include "esp_websocket_client.h"
#undef send(a, b, c, d)

struct WebsocketPayload {
    char* buffer;
    size_t len;

    WebsocketPayload(char mode, char* buffer, size_t len) {
        this->buffer = new char[len + 1];
        this->buffer[0] = mode;
        for (size_t i = 0; i < len; i++) {
            this->buffer[i + 1] = buffer[i];
        }
        this->len = len + 1;
    }

    ~WebsocketPayload() {
        delete[] buffer;
    }
};

class Websocket {
   private:
    esp_websocket_client_handle_t websocketClient;
    esp_websocket_client_config_t ws_cfg;

   public:
    Websocket(const char* uri, int port) {
        ws_cfg = esp_websocket_client_config_t{
            .uri = uri,
            .port = port,
        };
        init();
    }

    ~Websocket() {
        destroy();
    }

    void init() {
        websocketClient = esp_websocket_client_init(&ws_cfg);
    }

    esp_err_t start() {
        return esp_websocket_client_start(websocketClient);
    }

    esp_err_t stop() {
        return esp_websocket_client_stop(websocketClient);
    }

    esp_err_t destroy() {
        return esp_websocket_client_destroy(websocketClient);
    }

    int send(char* data) {
        return esp_websocket_client_send(websocketClient, data, strlen(data), portMAX_DELAY);
    }

    int send(char* data, size_t len) {
        return esp_websocket_client_send(websocketClient, data, len, portMAX_DELAY);
    }

    bool isConnected() {
        return esp_websocket_client_is_connected(websocketClient);
    }

    void registerDataEventHandler(esp_event_handler_t handler, void* arg) {
        esp_websocket_register_events(websocketClient, WEBSOCKET_EVENT_DATA, handler, arg);
    }

    void registerEventHandler(esp_websocket_event_id_t event, esp_event_handler_t handler, void* arg) {
        esp_websocket_register_events(websocketClient, event, handler, arg);
    }
};
#endif

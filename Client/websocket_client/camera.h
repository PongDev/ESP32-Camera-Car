#pragma once

#ifndef CAMERA_H
#define CAMERA_H
#include "esp_camera.h"
#include "websocket.h"

WebsocketPayload* getCameraImagePayload(char payloadHeader) {
    camera_fb_t* fb = esp_camera_fb_get();
    WebsocketPayload* payload = new WebsocketPayload(payloadHeader, (char*)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return payload;
}
#endif

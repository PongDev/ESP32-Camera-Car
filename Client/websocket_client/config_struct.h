#ifndef CONFIG_STRUCT_H
#define CONFIG_STRUCT_H
#include "esp32-hal-gpio.h"

struct WifiConfigStruct {
    const char* ssid;
    const char* password;
};

struct WebsocketConfigStruct {
    const char* uri;
    const int port;
};

struct MotorPin {
    uint8_t motorPWMPin;
    uint8_t enbLeftForwardPin;
    uint8_t enbLeftBackwardPin;
    uint8_t enbRightForwardPin;
    uint8_t enbRightBackwardPin;
};

struct UltraSonicPin {
    uint8_t trigPin;
    uint8_t echoPin;
};

struct Config {
    WifiConfigStruct wifi;
    WebsocketConfigStruct websocket;
    MotorPin motorPin;
    UltraSonicPin ultraSonicPin;
    unsigned int fps;
    const char* cameraName;
};
#endif

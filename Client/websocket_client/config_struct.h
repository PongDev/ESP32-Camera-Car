#ifndef CONFIG_STRUCT_H
#define CONFIG_STRUCT_H
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

struct Config {
    WifiConfigStruct wifi;
    WebsocketConfigStruct websocket;
    MotorPin motorPin;
    unsigned int fps;
    const char* cameraName;
};
#endif
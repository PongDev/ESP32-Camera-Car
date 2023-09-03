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

struct Config {
    WifiConfigStruct wifi;
    WebsocketConfigStruct websocket;
    unsigned int fps;
};
#endif
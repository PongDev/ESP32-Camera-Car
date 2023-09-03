#ifndef CONFIG_H
#define CONFIG_H
#include "config_struct.h"

Config config{
    wifi : WifiConfigStruct{
        "Wifi SSID",
        "Wifi Password",
    },
    websocket : WebsocketConfigStruct{
        uri : "ws://websocket.domain",
        port : 80,
    },
    fps : 10,
};
#endif

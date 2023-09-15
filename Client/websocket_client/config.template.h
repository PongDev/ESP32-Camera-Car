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
    motorPin : MotorPin{
        motorPWMPin : 0,
        enbLeftForwardPin : 0,
        enbLeftBackwardPin : 0,
        enbRightForwardPin : 0,
        enbRightBackwardPin : 0,
    },
    fps : 10,
    cameraName : "Camera Name",
};
#endif

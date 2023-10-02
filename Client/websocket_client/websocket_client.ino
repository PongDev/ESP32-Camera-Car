#include <WiFi.h>

#include "camera.h"
#include "config.h"
#include "esp_camera.h"
#include "led.h"
#include "motor.h"
#include "ultrasonic.h"
#include "websocket.h"

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
// #define CAMERA_MODEL_WROVER_KIT // Has PSRAM
// #define CAMERA_MODEL_ESP_EYE  // Has PSRAM
// #define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
// #define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
// #define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
// #define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
// #define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
// #define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER  // Has PSRAM
// #define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
// #define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
//  ** Espressif Internal Boards **
// #define CAMERA_MODEL_ESP32_CAM_BOARD
// #define CAMERA_MODEL_ESP32S2_CAM_BOARD
// #define CAMERA_MODEL_ESP32S3_CAM_LCD
// #define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
// #define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"

void startCameraServer();
Websocket* websocket = NULL;
Motor* motor = NULL;
// UltraSonic* ultraSonic = NULL;
unsigned long lastSendImageTime = 0;
// unsigned long lastSendDistanceTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    camera_config_t cameraConfig;
    cameraConfig.ledc_channel = LEDC_CHANNEL_0;
    cameraConfig.ledc_timer = LEDC_TIMER_0;
    cameraConfig.pin_d0 = Y2_GPIO_NUM;
    cameraConfig.pin_d1 = Y3_GPIO_NUM;
    cameraConfig.pin_d2 = Y4_GPIO_NUM;
    cameraConfig.pin_d3 = Y5_GPIO_NUM;
    cameraConfig.pin_d4 = Y6_GPIO_NUM;
    cameraConfig.pin_d5 = Y7_GPIO_NUM;
    cameraConfig.pin_d6 = Y8_GPIO_NUM;
    cameraConfig.pin_d7 = Y9_GPIO_NUM;
    cameraConfig.pin_xclk = XCLK_GPIO_NUM;
    cameraConfig.pin_pclk = PCLK_GPIO_NUM;
    cameraConfig.pin_vsync = VSYNC_GPIO_NUM;
    cameraConfig.pin_href = HREF_GPIO_NUM;
    cameraConfig.pin_sccb_sda = SIOD_GPIO_NUM;
    cameraConfig.pin_sccb_scl = SIOC_GPIO_NUM;
    cameraConfig.pin_pwdn = PWDN_GPIO_NUM;
    cameraConfig.pin_reset = RESET_GPIO_NUM;
    cameraConfig.xclk_freq_hz = 20000000;
    cameraConfig.frame_size = FRAMESIZE_UXGA;
    cameraConfig.pixel_format = PIXFORMAT_JPEG;  // for streaming
    // cameraConfig.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    cameraConfig.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    cameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
    cameraConfig.jpeg_quality = 12;
    cameraConfig.fb_count = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (cameraConfig.pixel_format == PIXFORMAT_JPEG) {
        if (psramFound()) {
            cameraConfig.jpeg_quality = 10;
            cameraConfig.fb_count = 2;
            cameraConfig.grab_mode = CAMERA_GRAB_LATEST;
        } else {
            // Limit the frame size when PSRAM is not available
            cameraConfig.frame_size = FRAMESIZE_SVGA;
            cameraConfig.fb_location = CAMERA_FB_IN_DRAM;
        }
    } else {
        // Best option for face detection/recognition
        cameraConfig.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
        cameraConfig.fb_count = 2;
#endif
    }

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // camera init
    esp_err_t err = esp_camera_init(&cameraConfig);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t* s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1);        // flip it back
        s->set_brightness(s, 1);   // up the brightness just a bit
        s->set_saturation(s, -2);  // lower the saturation
    }
    // drop down frame size for higher initial frame rate
    if (cameraConfig.pixel_format == PIXFORMAT_JPEG) {
        s->set_framesize(s, FRAMESIZE_QVGA);
    }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
    s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
    Led::setupLedFlash(LED_GPIO_NUM);
#endif

    WiFi.begin(config.wifi.ssid, config.wifi.password);
    WiFi.setSleep(false);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    startCameraServer();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    // Setup Motor Pin
    motor = new Motor(
        config.motorPin.motorPWMPin,
        config.motorPin.enbLeftForwardPin,
        config.motorPin.enbLeftBackwardPin,
        config.motorPin.enbRightForwardPin,
        config.motorPin.enbRightBackwardPin);

    // Setup Ultra Sonic Pin
    // ultraSonic = new UltraSonic(config.ultraSonicPin.trigPin, config.ultraSonicPin.echoPin);
}

void dataEventHandler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    esp_websocket_event_data_t* websocketData = (esp_websocket_event_data_t*)data;
    char* message = NULL;
    int ledDuty;
    int motorDuty;

    if (websocketData->data_len > 0) {
        Serial.printf("Received command mode: %d, Length: %d\n", (char)websocketData->data_ptr[0], websocketData->data_len);
        switch (websocketData->data_ptr[0]) {
            case 0:
                message = new char[websocketData->data_len];
                for (int i = 0; i < websocketData->data_len; i++) {
                    message[i] = websocketData->data_ptr[i + 1];
                }
                message[websocketData->data_len - 1] = 0;
                Serial.printf("Receive Message: %s\n", message);
                delete[] message;
                break;
            case 1:
                ledDuty = 0;
                for (int i = 1; i < websocketData->data_len; i++) {
                    ledDuty *= 10;
                    ledDuty += websocketData->data_ptr[i] - '0';
                }
                Led::setLedDuty(ledDuty);
                break;
            case 2:
                switch (websocketData->data_ptr[1]) {
                    case 'S':
                        motorDuty = 0;
                        for (int i = 2; i < websocketData->data_len; i++) {
                            motorDuty *= 10;
                            motorDuty += websocketData->data_ptr[i] - '0';
                        }
                        motor->setMotorPWMDuty(motorDuty);
                        break;
                    case 'X':
                        motor->stop();
                        break;
                    case 'F':
                        motor->moveFront();
                        break;
                    case 'B':
                        motor->moveBack();
                        break;
                    case 'L':
                        motor->moveLeft();
                        break;
                    case 'R':
                        motor->moveRight();
                        break;
                }
                break;
        }
        Serial.printf("Base: %s\nID: %d\nData: ", base, id);
        for (int i = 0; i < websocketData->data_len; i++) {
            Serial.printf("%c", websocketData->data_ptr[i]);
        }
        Serial.printf("\n\n");
    }
}

void resetWebsocket() {
    websocket->stop();
    delete websocket;
    websocket = NULL;
    Serial.println("Websocket reset");
    delay(1000);
}

void sendWebsocketPayload(WebsocketPayload* payload) {
    try {
        websocket->send(payload->buffer, payload->len);
        delete payload;
    } catch (...) {
        delete payload;
        resetWebsocket();
    }
}

void loop() {
    if (websocket == NULL) {
        websocket = new Websocket(config.websocket.uri, config.websocket.port);
        websocket->registerDataEventHandler(&dataEventHandler, NULL);
        if (websocket->start() != ESP_OK) {
            Serial.println("Websocket not started");
            websocket->stop();
            delete websocket;
            websocket = NULL;
        } else {
            Serial.println("Websocket started");
        }
        delay(1000);
        sendWebsocketPayload(new WebsocketPayload((char)0, (char*)config.cameraName, strlen(config.cameraName)));
    } else if (!websocket->isConnected()) {
        resetWebsocket();
    } else {
        if (millis() - lastSendImageTime > (1000 / config.fps == 0 ? 1 : config.fps)) {
            sendWebsocketPayload(getCameraImagePayload((char)1));
            lastSendImageTime = millis();
        }
        // if (millis() - lastSendDistanceTime > 100) {
        //     sendWebsocketPayload(ultraSonic->getDistancePayload((char)2));
        //     lastSendDistanceTime = millis();
        // }
    }
    if (millis() - motor->getLastActionTime() > 250) {
        motor->stop();
    }
}

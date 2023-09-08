#pragma once

#ifndef LED_H
#define LED_H
#include "esp32-hal-ledc.h"

#define LED_LEDC_CHANNEL 2
#define CONFIG_LED_MAX_INTENSITY 255

class Led {
   public:
    static void setupLedFlash(int pin) {
        ledcSetup(LED_LEDC_CHANNEL, 5000, 8);
        ledcAttachPin(pin, LED_LEDC_CHANNEL);
        ledcWrite(LED_LEDC_CHANNEL, 0);
    }

    static void setLedDuty(int ledDuty) {
        if (ledDuty < 0) {
            ledDuty = 0;
        } else if (ledDuty > CONFIG_LED_MAX_INTENSITY) {
            ledDuty = CONFIG_LED_MAX_INTENSITY;
        }
        ledcWrite(LED_LEDC_CHANNEL, ledDuty);
    }
};
#endif
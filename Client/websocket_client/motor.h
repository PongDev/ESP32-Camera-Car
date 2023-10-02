#pragma once

#ifndef MOTOR_H
#define MOTOR_H
#include "esp32-hal-gpio.h"
#include "esp32-hal-ledc.h"

#define MOTOR_LEDC_CHANNEL 3
#define CONFIG_MOTOR_MAX_INTENSITY 255

class Motor {
   private:
    uint8_t enbLeftForwardPin;
    uint8_t enbLeftBackwardPin;
    uint8_t enbRightForwardPin;
    uint8_t enbRightBackwardPin;
    unsigned long lastActionTime;

   public:
    Motor(uint8_t pwmPin,
          uint8_t enbLeftForwardPin,
          uint8_t enbLeftBackwardPin,
          uint8_t enbRightForwardPin,
          uint8_t enbRightBackwardPin) {
        ledcSetup(MOTOR_LEDC_CHANNEL, 5000, 8);
        ledcAttachPin(pwmPin, MOTOR_LEDC_CHANNEL);
        ledcWrite(MOTOR_LEDC_CHANNEL, 0);

        this->enbLeftForwardPin = enbLeftForwardPin;
        this->enbLeftBackwardPin = enbLeftBackwardPin;
        this->enbRightForwardPin = enbRightForwardPin;
        this->enbRightBackwardPin = enbRightBackwardPin;

        pinMode(this->enbLeftForwardPin, OUTPUT);
        pinMode(this->enbLeftBackwardPin, OUTPUT);
        pinMode(this->enbRightForwardPin, OUTPUT);
        pinMode(this->enbRightBackwardPin, OUTPUT);

        this->lastActionTime = millis();
    }

    void setMotorPWMDuty(int motorDuty) {
        if (motorDuty < 0) {
            motorDuty = 0;
        } else if (motorDuty > CONFIG_MOTOR_MAX_INTENSITY) {
            motorDuty = CONFIG_MOTOR_MAX_INTENSITY;
        }
        ledcWrite(MOTOR_LEDC_CHANNEL, motorDuty);
        lastActionTime = millis();
    }

    void stop() {
        digitalWrite(enbLeftForwardPin, LOW);
        digitalWrite(enbLeftBackwardPin, LOW);
        digitalWrite(enbRightForwardPin, LOW);
        digitalWrite(enbRightBackwardPin, LOW);
        lastActionTime = millis();
    }

    void moveFront() {
        digitalWrite(enbLeftForwardPin, HIGH);
        digitalWrite(enbLeftBackwardPin, LOW);
        digitalWrite(enbRightForwardPin, HIGH);
        digitalWrite(enbRightBackwardPin, LOW);
        lastActionTime = millis();
    }

    void moveBack() {
        digitalWrite(enbLeftForwardPin, LOW);
        digitalWrite(enbLeftBackwardPin, HIGH);
        digitalWrite(enbRightForwardPin, LOW);
        digitalWrite(enbRightBackwardPin, HIGH);
        lastActionTime = millis();
    }

    void moveLeft() {
        digitalWrite(enbLeftForwardPin, LOW);
        digitalWrite(enbLeftBackwardPin, HIGH);
        digitalWrite(enbRightForwardPin, HIGH);
        digitalWrite(enbRightBackwardPin, LOW);
        lastActionTime = millis();
    }

    void moveRight() {
        digitalWrite(enbLeftForwardPin, HIGH);
        digitalWrite(enbLeftBackwardPin, LOW);
        digitalWrite(enbRightForwardPin, LOW);
        digitalWrite(enbRightBackwardPin, HIGH);
        lastActionTime = millis();
    }

    unsigned long getLastActionTime() {
        return lastActionTime;
    }
};
#endif

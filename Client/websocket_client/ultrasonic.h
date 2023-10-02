#pragma once

#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include "esp32-hal-gpio.h"
#include "websocket.h"
#include "wiring_private.h"

class UltraSonic {
   private:
    uint8_t trigPin;
    uint8_t echoPin;

   public:
    UltraSonic(uint8_t trigPin, uint8_t echoPin) {
        this->trigPin = trigPin;
        this->echoPin = echoPin;

        pinMode(this->trigPin, OUTPUT);
        pinMode(this->echoPin, INPUT);
    }

    double getDistance() {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);

        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        unsigned long duration = pulseIn(echoPin, HIGH);

        return (duration * 0.034) / 2;
    }

    WebsocketPayload* getDistancePayload(char payloadHeader) {
        char distanceString[32];

        sprintf(distanceString, "%lf", this->getDistance());
        WebsocketPayload* payload = new WebsocketPayload(payloadHeader, distanceString, strlen(distanceString));
        return payload;
    }
};
#endif

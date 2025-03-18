#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>

#define BUTTON_PIN 13
#define LED_PIN_1 2
#define LED_PIN_2 14
#define LED_PIN_3 12

#define LED_COUNT 3  

const int ledPins[LED_COUNT] = {LED_PIN_1, LED_PIN_2, LED_PIN_3};

#define MIN_BLINK_DELAY 50
#define MAX_BLINK_DELAY 1000
#define BLINK_STEP 200  

volatile int blinkDelay = MAX_BLINK_DELAY;
unsigned long previousMillis = 0;
int ledIndex = 0;  

bool lastButtonState = HIGH;
bool currentButtonState;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void initPins() {
    for (int i = 0; i < LED_COUNT; i++) {
        pinMode(ledPins[i], OUTPUT);
    }
}

void blinkLeds() {
    static int state = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= blinkDelay) {
        previousMillis = currentMillis;

        // Вимикаємо всі світлодіоди перед зміною стану
        for (int i = 0; i < LED_COUNT; i++) {
            digitalWrite(ledPins[i], LOW);
        }

        // Вмикаємо світлодіоди відповідно до потрібного порядку
        if (state == 0) {
            digitalWrite(LED_PIN_1, HIGH);
            digitalWrite(LED_PIN_2, HIGH);
        } else if (state == 1) {
            digitalWrite(LED_PIN_3, HIGH);
            digitalWrite(LED_PIN_2, HIGH);
        } else if (state == 2) {
            digitalWrite(LED_PIN_3, HIGH);
            digitalWrite(LED_PIN_1, HIGH);
        }

        // Переходимо до наступного стану
        state = (state + 1) % 3;
    }
}

void checkButton() {
    currentButtonState = digitalRead(BUTTON_PIN);

    if (currentButtonState != lastButtonState) {
        if ((millis() - lastDebounceTime) > debounceDelay) {  
            lastDebounceTime = millis();  

            if (currentButtonState == LOW) {  
                if (blinkDelay > MIN_BLINK_DELAY) {
                    blinkDelay -= BLINK_STEP;
                } else {
                    blinkDelay = MAX_BLINK_DELAY;
                }
                Serial.print("New blink speed from button: ");
                Serial.println(blinkDelay);
            }
        }
        lastButtonState = currentButtonState;  
    }
}

#endif

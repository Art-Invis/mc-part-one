#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "wifi.h"
#include "ledControl.h"
#include "webPage.h"


AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);  
    pinMode(BUTTON_PIN, INPUT);  

    initPins();
    initWiFi();
    initServer(server);
}

void loop() {
    checkButton();  
    blinkLeds();  
}

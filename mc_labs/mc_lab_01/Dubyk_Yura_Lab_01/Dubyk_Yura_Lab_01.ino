#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define ESP_WIFI_MODE 2  // WIFI_STA // WIFI_AP //WIFI_AP_STA
#define DELAY_BETWEEN_BUTTONS 800

// Replace with your network credentials
const char *ssid = "WEMOS_ESP8266_Yura";
const char *password = "IoT";

// Assign GPIO to the relay and button
const int LED1 = D5;           // Replace with the desired GPIO pin
const int LED2 = LED_BUILTIN;  // Replace with the desired GPIO pin
const int LED3 = D6;           // Replace with the desired GPIO pin

const int btnGPIO = D7;  // Replace with the desired GPIO pin

short int ledsArray[] = { LED1, LED2, LED3 };
short int reverseLedsArray[] = { LED3, LED2, LED1 };
short int currentLedsArray[3];
uint8_t ledLen = sizeof(ledsArray) / sizeof(ledsArray[0]);
uint32_t timestamp;
uint8_t currentLed = 0;
uint8_t prevLed;
uint8_t debounceDelay = 20;

bool prevState;
bool presentState;
bool btnPressed = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial;
            text-align: center;
            margin: 0px auto;
            padding-top: 30px;
        }

        .button {
            padding: 10px 20px;
            font-size: 24px;
            text-align: center;
            outline: none;
            color: #fff;
            background-color: #2f4468;
            border: none;
            border-radius: 5px;
            box-shadow: 0 6px #999;
            cursor: pointer;
            -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
        }

        .button:hover {
            background-color: #1f2e45
        }

        .button:active {
            background-color: #1f2e45;
            box-shadow: 0 4px #666;
            transform: translateY(2px);
        }

        .leds {
            width: 100px;
            height: 100px;
            margin: 20px;
            display: inline-block;
            border: 5px solid black;
            border-radius: 50%;
        }

        .container {
            text-align: center;
        }

        #led1 {
            background-color: white;
        }

        @media screen and (max-width: 480px) {
            .leds {
                width: 50px;
                height: 50px;
            }

            .button {
                padding: 15px 100px 15px 10px;
                font-size: 10px;
            }

            h1 {
                font-size: 24px;
                padding-top: 20px;
            }
        }
    </style>
</head>

<body>
    <h1>ESP Pushbutton Web Server</h1>
    <!-- onmousedown / onmouseup - on PC/Laptop, ontouchend / ontouchstart - on mobile -->
    <button class="button" onmousedown="algorighm1('on_alg1');" ontouchstart="algorighm1('on_alg1');"
        onmouseup="algorighm1('off_alg1');" ontouchend="algorighm1('off_alg1');">Algorithm 1</button>
    <button class="button" onmousedown="algorighm2('on');" ontouchstart="algorighm2('on');"
        onmouseup="algorighm2('off');" ontouchend="algorighm2('off');">Algorithm 2</button>

    <div class="container">
        <div id="led3" class="leds"></div>
        <div id="led2" class="leds"></div>
        <div id="led1" class="leds"></div>
    </div>

    <script>
        var led1 = document.getElementById('led1');
        var led2 = document.getElementById('led2');
        var led3 = document.getElementById('led3');

        function toggleCheckLed1(element) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/status_led_1", true);
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    var status_led_1 = xhr.responseText;
                    console.log("Led1 status:", status_led_1);
                    if (status_led_1 === '1') {
                        led1.style.backgroundColor = 'red';
                    } else {
                        led1.style.backgroundColor = 'white';
                    }
                }
            };
            xhr.send();
        }

        function toggleCheckLed2(element) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/status_led_2", true);
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    var status_led_2 = xhr.responseText;
                    console.log("Led2 status:", status_led_2);

                    if (status_led_2 === '1') {
                        led2.style.backgroundColor = 'orange';
                    } else {
                        led2.style.backgroundColor = 'white';
                    }
                }
            };
            xhr.send();
        }

        function toggleCheckLed3(element) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/status_led_3", true);
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    var status_led_3 = xhr.responseText;
                    console.log("Led3 status:", status_led_3);
                    if (status_led_3 === '1') {
                        led3.style.backgroundColor = 'green';
                    } else {
                        led3.style.backgroundColor = 'white';
                    }
                }
            };
            xhr.send();
        }
        setInterval(toggleCheckLed1, 100);
        setInterval(toggleCheckLed2, 100);
        setInterval(toggleCheckLed3, 100);

        function algorighm1(x) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/" + x, true);
            xhr.send();
        }

        function algorighm2(x) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/" + x, true);
            xhr.send();
        }
    </script>
</body>

</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initWiFi() {

  if (ESP_WIFI_MODE == 1) {
    WiFi.mode(WIFI_STA);
    // Connect to Wi-Fi network with SSID and password
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println();
    Serial.print("ESP IP Address: http://");
    Serial.println(WiFi.localIP());

    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
  } else {  // (ESP_WIFI_MODE == WIFI_AP)
    WiFi.mode(WIFI_AP);
    Serial.println("Setting AP (Access Point)…");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  }
  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request
  server.on("/on_alg1", HTTP_GET, [](AsyncWebServerRequest *request) {
    btnPressed = true;
    request->send(200, "text/plain", "ok");
  });

  server.on("/status_led_1", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(LED1)).c_str());
  });

  server.on("/status_led_2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(LED2)).c_str());
  });

  server.on("/status_led_3", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(LED3)).c_str());
  });

  // Receive an HTTP GET request
  server.on("/off_alg1", HTTP_GET, [](AsyncWebServerRequest *request) {
    btnPressed = false;
    request->send(200, "text/plain", "ok");
  });

  server.onNotFound(notFound);
  server.begin();
}

IRAM_ATTR void btnChange() {
  prevState = digitalRead(btnGPIO);
  delay(20);
  presentState = digitalRead(btnGPIO);
  if (presentState == prevState) {
    if (presentState == LOW) {
      btnPressed = true;
    } else {
      btnPressed = false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(btnGPIO, INPUT);
  attachInterrupt(digitalPinToInterrupt(btnGPIO), btnChange, CHANGE);
  timestamp = millis();
  initWiFi();
}

void choose_led_array() {
  if (btnPressed) {
    for (int i = 0; i < ledLen; i++) {
      currentLedsArray[i] = reverseLedsArray[i];
    }
  } else {
    for (int i = 0; i < ledLen; i++) {
      currentLedsArray[i] = ledsArray[i];
    }
  }
}

void do_algorithm() {
  choose_led_array();
  if (prevLed != 999) {
    prevLed = currentLed;
  } else prevLed = 0;
  digitalWrite(currentLedsArray[currentLed], HIGH);
  currentLed++;
  if (currentLed >= ledLen) {
    currentLed = 0;
  }
}

void loop() {
  if (millis() - timestamp >= DELAY_BETWEEN_BUTTONS) {
    timestamp = millis();
    for (int i = 0; i < ledLen; i++) {
      digitalWrite(ledsArray[i], LOW);
    }
    do_algorithm();
  }
}

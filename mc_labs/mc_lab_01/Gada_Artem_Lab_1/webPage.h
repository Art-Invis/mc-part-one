#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; text-align: center; margin: 0 auto; padding-top: 30px; }
        .button { padding: 10px 20px; font-size: 24px; color: #fff; border: none; border-radius: 5px; cursor: pointer; margin: 10px; }
        .increase { background-color: #28a745; } /* Зелена */
        .increase:hover { background-color: #218838; }
        .reset { background-color: #ff9800; } /* Помаранчева */
        .reset:hover { background-color: #e68900; }
    </style>
</head>
<body>
    <h1>ESP8266 LED Speed Control</h1>
    <button class="button increase" onclick="increaseSpeed()">Increase Speed</button>
    <button class="button reset" onclick="resetSpeed()">Reset Speed</button>
    <p id="speedLabel">Current Speed: 1000 ms</p>
    <script>
        function increaseSpeed() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/increaseSpeed", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    document.getElementById("speedLabel").innerText = "Current Speed: " + xhr.responseText + " ms";
                }
            };
            xhr.send();
        }

        function resetSpeed() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/resetSpeed", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    document.getElementById("speedLabel").innerText = "Current Speed: " + xhr.responseText + " ms";
                }
            };
            xhr.send();
        }

        function getDelayValue() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/get_delay", true);
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    document.getElementById("speedLabel").innerText = "Current Speed: " + xhr.responseText + " ms";
                }
            };
            xhr.send();
        }

        setInterval(getDelayValue, 1000);
    </script>
</body>
</html>
)rawliteral";

void initServer(AsyncWebServer &server) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);  
    });

    server.on("/increaseSpeed", HTTP_GET, [](AsyncWebServerRequest *request){
        if (blinkDelay > MIN_BLINK_DELAY) {
            blinkDelay -= 200;  
        } else {
            blinkDelay = MAX_BLINK_DELAY;  
        }
        Serial.print("New blink speed: ");
        Serial.println(blinkDelay);
        request->send(200, "text/plain", String(blinkDelay));
    });

    server.on("/resetSpeed", HTTP_GET, [](AsyncWebServerRequest *request){
        blinkDelay = MAX_BLINK_DELAY;  
        Serial.println("Speed reset to 1000 ms");
        request->send(200, "text/plain", String(blinkDelay));
    });

    server.on("/get_delay", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(blinkDelay));
    });

    server.begin();
}

#endif

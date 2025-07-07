#include "arduino_secrets.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <U8g2lib.h>

// OLED I2C Set Up with u8g2 library (I prefer over Adafruits SSD1308)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// AP credentials
const char* ssid = "ESP32-Control";
const char* password = "12345678";

// Async Web Server on port 80
AsyncWebServer server(80);

// Last command displays on OLED
String lastCommand = "";

void displayCommand(const String& command) {
  lastCommand = command;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 20, "Last Command:");
  u8g2.drawStr(0, 40, command.c_str());
  u8g2.sendBuffer();
}

// HTML Webpage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Learning Mode</title>
  <style>
    body {
      background-color: pink;
      font-family: Arial, sans-serif;
      text-align: center;
      padding-top: 50px;
    }
    h1 {
      color: #333;
    }
    button {
      padding: 15px 30px;
      margin: 10px;
      font-size: 18px;
      background-color: #fff;
      border: 2px solid #333;
      border-radius: 8px;
      cursor: pointer;
    }
    button:hover {
      background-color: #f0f0f0;
    }
  </style>
</head>
<body>
  <h1>Learning Mode</h1>
  <button onclick="sendCommand('Left 10Â°')">Turn Left 10Â°</button>
  <button onclick="sendCommand('Right 10Â°')">Turn Right 10Â°</button><br>
  <button onclick="sendCommand('Forward 10cm')">Forward 10cm</button>
  <button onclick="sendCommand('Backward 10cm')">Backward 10cm</button>
  <script>
    function sendCommand(cmd) {
      fetch("/command?cmd=" + cmd);
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  // Initialize I2C OLED
  u8g2.begin();
  displayCommand("Waiting...");

  // Start AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Serve HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Handle command presses
  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      Serial.println("Command: " + cmd);
      displayCommand(cmd);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  // Nothing as it is handled by Asynchronous Server
}
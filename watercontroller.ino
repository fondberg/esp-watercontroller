#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#ifndef STASSID
 #error "SSID not defined NOT DEFINED"
#endif

// ================= Common variables
#define WATERCONTROLLER_VERSION "1.2"
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int BUTTON_PIN = 4;
const int BLUE_LED_PIN = 13;
const int RELAY_PIN = 15;

const long AUTO_OFF_DELAY = 600000; // turn on for 10 mins
const int LOOP_DELAY = 100;
unsigned long turnedOnMillis = 0;

// ================= SETUP
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  // Wait for connection
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("nodemcuv3-watercontroller")) {
    Serial.println("MDNS responder started");
  }

  // initiliaze OTA
  setupOta();
  
  server.on("/status", handleStatus);
  server.on("/on", handleWaterSwitchOn);
  server.on("/off", handleWaterSwitchOff);
  server.on("/ha-switch", handleHomeassistantSwitch);
  server.onNotFound(handleNotFound);
  server.begin();
}


void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  MDNS.update();
  
  unsigned long currentMillis = millis();
  // check button
  int buttonState = digitalRead(BUTTON_PIN);
  int relayState = digitalRead(RELAY_PIN);

  if (buttonState == HIGH) {
    if (relayState == LOW) {
      turnOnWater();
    } else {
      turnOffWater();
    }
  } else {
    // turn off relay automatically after timeout
    if (relayState == HIGH && currentMillis - turnedOnMillis >= AUTO_OFF_DELAY) {
      turnOffWater();
    }
  }
  delay(LOOP_DELAY);
}

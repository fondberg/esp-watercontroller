void handleWaterSwitchOn() {
  turnOnWater();
  handleStatus();
}

void handleWaterSwitchOff() {
  turnOffWater();
  handleStatus();
}

void handleNotFound() {
  String message = "Endpoint not implemented\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// ================= 
void handleStatus() {
  
  int relayState = digitalRead(RELAY_PIN);
  // String onoff = relayState == HIGH ? "on" : "off";

  StaticJsonDocument<1024> doc;
  doc["is_active"] = (relayState == HIGH);
  doc["currentMillis"] = millis();
  doc["turnedOnMillis"] = turnedOnMillis;
  doc["autoTurnOffDelay"] = AUTO_OFF_DELAY;
  doc["version"] = WATERCONTROLLER_VERSION;
  String msg;
  serializeJson(doc, msg);
  
  server.send(200, "application/json", msg);
}

void handleHomeassistantSwitch() {
  if(server.method() == HTTP_POST) {
    if (server.hasArg("plain") == false) {
      server.send(422, "text/plain", "body missing");
      return;
    }
    String body = server.arg("plain");

    StaticJsonDocument<1024> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, body);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      String error = "Failed to parse json";
      error += error.c_str();
      server.send(501, "text/plain", error);
      return;
    }

    if(doc.containsKey("active") == false) {
      server.send(422, "text/plain", "json key 'active' is missing");
      return;
    }
    const char* active = doc["active"];
  
    if(strcmp(active, "true") == 0) {
      handleWaterSwitchOn();
    } else {
      handleWaterSwitchOff();
    }
    return;
  }
  // Handle non POST requests
  handleStatus();
}

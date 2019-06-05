// ================= Common functions
void turnOnWater() {
  // Should it toggle or extend the relay, right now it extends it?
  digitalWrite(BLUE_LED_PIN, HIGH);
  digitalWrite(RELAY_PIN, HIGH);
  turnedOnMillis = millis();
  Serial.println("Turned ON");
}

void turnOffWater() {
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  turnedOnMillis = 0;
  Serial.println("Turned OFF");
}

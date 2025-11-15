#include <Arduino.h>

// Placeholder sketch outlining the high-level loop for the Swift Cabin Controller.
// Sensors, networking, and relay logic will be implemented in upcoming iterations.

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Swift Cabin Controller booting...");
}

void loop() {
  Serial.println("Telemetry/command cycle placeholder");
  delay(60000); // sleep placeholder until deep sleep is implemented
}

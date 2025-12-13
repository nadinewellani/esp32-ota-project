#include <WiFi.h>

// Pin de la LED intégrée ESP32 (généralement GPIO 2)
// Si votre ESP32 a la LED sur une autre pin, modifiez cette valeur
#define LED_BUILTIN 2

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Firmware Version 2.0");
  
  // Configuration de la LED intégrée
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Allumer la LED
  Serial.println("LED intégrée allumée");
}

void loop() {
  Serial.println("Running V2...");
  // La LED reste allumée
  delay(1500);
}

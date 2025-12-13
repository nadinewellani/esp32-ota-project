#include <WiFi.h>
#include <DHT.h>

// Configuration du capteur DHT11
#define DHTPIN 4       // Pin D4 (GPIO 4) de l'ESP32 - DATA du DHT11 connecté ici
#define DHTTYPE DHT22   // Type de capteur

// Initialiser le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Firmware Version 1.0");
  Serial.println("Initialisation du capteur DHT11...");
  
  // Démarrer le capteur DHT11
  dht.begin();
  delay(2000); // Attendre que le capteur soit prêt
  
  Serial.println("DHT11 initialisé");
}

void loop() {
  // Attendre 2 secondes entre les mesures (minimum requis pour DHT11)
  delay(2000);
  
  // Lire l'humidité (en %)
  float humidity = dht.readHumidity();
  
  // Lire la température (en Celsius)
  float temperature = dht.readTemperature();
  
  // Vérifier si la lecture a échoué
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Erreur de lecture du capteur DHT11 !");
    return;
  }
  
  // Afficher les valeurs
  Serial.println("=== Lecture DHT11 ===");
  Serial.print("Humidité: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Température: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  // Calculer l'index de chaleur (heat index) en Celsius
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  Serial.print("Index de chaleur: ");
  Serial.print(heatIndex);
  Serial.println(" °C");
  Serial.println("====================");
}

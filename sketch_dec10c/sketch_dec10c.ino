/* ESP32 OTA sécurisé (HTTPS + vérif SHA256) avec système de Rollback et Webhooks GitHub
   
   Fonctionnalités:
   - Mise à jour OTA sécurisée via HTTPS
   - Vérification SHA256 du firmware
   - Système de rollback automatique si firmware défectueux
   - Utilisation des partitions OTA (ota_0 et ota_1)
   - Détection de redémarrages en boucle
   - Serveur web HTTP pour recevoir des webhooks GitHub
   - Mise à jour automatique déclenchée par GitHub
   - Polling amélioré (vérification toutes les 10 secondes)
   
   Système de Rollback:
   - Si le firmware redémarre plus de MAX_BOOT_FAILURES fois sans validation,
     le système bascule automatiquement vers l'autre partition OTA
   - Un boot est validé après BOOT_VALIDATION_TIME (30 secondes) de fonctionnement
   - Les deux versions (v1.0 et v2.0) peuvent coexister sur les deux partitions
   
   Système de Webhook GitHub:
   - Serveur HTTP sur le port 80
   - Endpoint /webhook pour recevoir les notifications GitHub
   - Endpoint /update pour forcer manuellement une vérification
   - Endpoint /status pour voir le statut du système
   - Mise à jour UNIQUEMENT sur déclenchement (webhook ou manuel)
   - AUCUNE vérification périodique automatique
   
   Configuration Webhook GitHub:
   1. Allez dans votre repo GitHub → Settings → Webhooks
   2. Cliquez sur "Add webhook"
   3. Payload URL: http://<IP_ESP32>/webhook
   4. Content type: application/json
   5. Events: Sélectionnez "Just the push event" ou "Let me select individual events"
   6. Activez le webhook
   
   Note: Pour que GitHub puisse accéder à l'ESP32, vous devez:
   - Utiliser un service de tunneling (ngrok, localtunnel, etc.)
   - Ou configurer un port forwarding sur votre routeur
   - Ou utiliser un serveur intermédiaire qui reçoit le webhook et notifie l'ESP32
   
   Dépendances:
    - ArduinoJson (install via Library Manager)
    - Partitions OTA configurées dans partitions.csv (ota_0 et ota_1)
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include "mbedtls/sha256.h" // fourni avec ESP32 core
#include <Preferences.h>
#include <esp_ota_ops.h>
#include <esp_app_format.h>
#include <WebServer.h>

// --- REMPLACE TES INFOS ---
const char* ssid = "Ooredoo-321536";
const char* password ="49876C5BSr!78";

String currentVersion = "1.0";

// Mode debug : désactiver la vérification SSL (moins sécurisé, pour tests uniquement)
const bool DEBUG_INSECURE_SSL = false; // Mettre à true pour désactiver la vérification SSL

// Configuration du système de rollback
Preferences preferences;
const char* PREF_NAMESPACE = "ota_system";
const char* PREF_BOOT_COUNT = "boot_count";
const char* PREF_BOOT_SUCCESS = "boot_success";
const char* PREF_CURRENT_PARTITION = "current_part";
const int MAX_BOOT_FAILURES = 3; // Nombre max de redémarrages avant rollback
const unsigned long BOOT_VALIDATION_TIME = 30000; // 30 secondes pour valider un boot réussi

// Configuration du serveur web pour recevoir les webhooks
WebServer server(80);
bool forceUpdateCheck = false; // Flag pour forcer une vérification immédiate

// Root CA : copie le contenu du fichier github_raw_cert.pem ici
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw
MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV
BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU
aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy
dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B
3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY
tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/
Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2
VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT
79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6
c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT
Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l
c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee
UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE
Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd
BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G
A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF
Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO
VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3
ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs
8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR
iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze
Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ
XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/
qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB
VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB
L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG
jjxDah2nGN59PRbxYvnKkKj9
-----END CERTIFICATE-----
)EOF";

// URL du fichier update.json (raw)
const char* updateJsonUrl = "https://raw.githubusercontent.com/nadinewellani/esp32-ota-project/main/update.json";

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Démarrage ESP32 OTA avec Rollback ===");
  
  // Initialiser les préférences pour le système de rollback
  preferences.begin(PREF_NAMESPACE, false);
  
  // Vérifier le système de rollback
  checkRollback();
  
  // Afficher la partition actuelle
  const esp_partition_t* running = esp_ota_get_running_partition();
  Serial.printf("Partition actuelle: %s (offset: 0x%08x)\n", running->label, running->address);
  
  // Incrémenter le compteur de boot
  int bootCount = preferences.getInt(PREF_BOOT_COUNT, 0) + 1;
  preferences.putInt(PREF_BOOT_COUNT, bootCount);
  Serial.printf("Compteur de boot: %d\n", bootCount);
  
  // Réinitialiser le flag de boot réussi (sera validé après BOOT_VALIDATION_TIME)
  preferences.putBool(PREF_BOOT_SUCCESS, false);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connexion au WiFi");

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connecté !");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
    delay(2000); // Attendre que la connexion soit stable
    
    // Démarrer le serveur web pour recevoir les webhooks
    setupWebServer();
    Serial.println("Serveur web démarré sur le port 80");
    Serial.println("Endpoints disponibles:");
    Serial.println("  - http://<IP>/update : Forcer une vérification de mise à jour");
    Serial.println("  - http://<IP>/webhook : Recevoir des webhooks GitHub");
    Serial.println("  - http://<IP>/status : Afficher le statut du système");
    Serial.println("\n=== MODE MISE À JOUR AUTOMATIQUE (ÉVÉNEMENTIEL) ===");
    Serial.println("La mise à jour se fera UNIQUEMENT via:");
    Serial.println("  1. Webhook GitHub (POST /webhook) - déclenchement automatique");
    Serial.println("  2. Requête manuelle (GET /update) - déclenchement manuel");
    Serial.println("AUCUNE vérification périodique ne sera effectuée.");
    Serial.println("En attente d'un déclenchement...");
  } else {
    Serial.println("\nERREUR : WiFi NON connecté.");
  }
  
  // Valider le boot après un délai (si on arrive ici, le firmware fonctionne)
  delay(BOOT_VALIDATION_TIME);
  validateBoot();
}

// Fonction pour valider un boot réussi
void validateBoot() {
  preferences.putBool(PREF_BOOT_SUCCESS, true);
  preferences.putInt(PREF_BOOT_COUNT, 0); // Réinitialiser le compteur après un boot réussi
  Serial.println("Boot validé avec succès !");
}

// Fonction pour vérifier et effectuer un rollback si nécessaire
void checkRollback() {
  int bootCount = preferences.getInt(PREF_BOOT_COUNT, 0);
  bool bootSuccess = preferences.getBool(PREF_BOOT_SUCCESS, false);
  
  Serial.printf("Vérification rollback: boot_count=%d, boot_success=%s\n", 
                bootCount, bootSuccess ? "true" : "false");
  
  // Si le compteur de boot est élevé et que le dernier boot n'a pas été validé
  if (bootCount >= MAX_BOOT_FAILURES && !bootSuccess) {
    Serial.println("!!! FIRMWARE DÉFECTUEUX DÉTECTÉ !!!");
    Serial.println("Tentative de rollback vers la partition précédente...");
    
    const esp_partition_t* running = esp_ota_get_running_partition();
    const esp_partition_t* update_partition = NULL;
    
    // Trouver l'autre partition OTA
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
      if (strcmp(running->label, "ota_0") == 0) {
        update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
      } else if (strcmp(running->label, "ota_1") == 0) {
        update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
      }
    }
    
    if (update_partition != NULL) {
      Serial.printf("Basculement vers partition: %s\n", update_partition->label);
      esp_ota_set_boot_partition(update_partition);
      preferences.putInt(PREF_BOOT_COUNT, 0); // Réinitialiser le compteur
      preferences.putBool(PREF_BOOT_SUCCESS, false);
      preferences.end();
      Serial.println("Redémarrage vers la partition précédente...");
      delay(1000);
      ESP.restart();
    } else {
      Serial.println("ERREUR: Impossible de trouver une partition de rollback !");
      Serial.println("Le système va continuer avec la partition actuelle.");
    }
  } else if (bootSuccess) {
    Serial.println("Boot précédent validé, pas de rollback nécessaire.");
  }
}

// Fonction pour convertir une URL GitHub en URL raw si nécessaire
String fixGitHubUrl(String url) {
  // Si c'est une URL GitHub blob, la convertir en raw
  if (url.indexOf("github.com") != -1 && url.indexOf("/blob/") != -1) {
    url.replace("/blob/", "/");
    url.replace("github.com", "raw.githubusercontent.com");
    Serial.printf("URL corrigée: %s\n", url.c_str());
  }
  return url;
}

String httpGetStringSecure(const char* url, WiFiClientSecure &client) {
  HTTPClient http;
  http.setTimeout(30000);
  http.begin(client, url);
  int code = http.GET();
  String payload = "";
  if (code == 200) payload = http.getString();
  else {
    Serial.printf("Erreur HTTP GET: %d\n", code);
  }
  http.end();
  return payload;
}

bool downloadAndUpdate(const char* url, const char* expectedSha256Hex) {
  Serial.printf("Téléchargement depuis: %s\n", url);
  
  WiFiClientSecure client;
  if (DEBUG_INSECURE_SSL) {
    client.setInsecure(); // Désactiver la vérification SSL (mode debug)
    Serial.println("ATTENTION: Mode debug - vérification SSL désactivée !");
  } else {
    client.setCACert(root_ca);
  }
  client.setTimeout(10000); // Timeout de 10 secondes
  
  HTTPClient http;
  http.setTimeout(30000); // Timeout HTTP de 30 secondes
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  
  if (!http.begin(client, url)) {
    Serial.println("Erreur: http.begin() a échoué");
    return false;
  }
  
  Serial.println("Connexion HTTPS en cours...");
  int code = http.GET();
  
  if (code != HTTP_CODE_OK) {
    Serial.printf("HTTP GET code: %d\n", code);
    if (code == -1) {
      Serial.println("Erreur -1: Problème de connexion HTTPS (certificat SSL ou timeout)");
      Serial.println("Vérifiez que l'URL est correcte et accessible");
    } else if (code < 0) {
      Serial.printf("Erreur HTTP: %s\n", http.errorToString(code).c_str());
    } else {
      String payload = http.getString();
      Serial.printf("Réponse serveur: %s\n", payload.substring(0, 200).c_str());
    }
    http.end();
    return false;
  }
  
  Serial.println("Connexion HTTPS réussie !");

  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Serial.println("Taille de fichier invalide");
    http.end();
    return false;
  }
  
  // Déterminer la partition OTA cible (utiliser l'autre partition que celle en cours)
  const esp_partition_t* running = esp_ota_get_running_partition();
  const esp_partition_t* update_partition = NULL;
  esp_ota_handle_t ota_handle = 0;
  bool use_ota_api = false;
  
  // Trouver l'autre partition OTA pour l'update
  if (strcmp(running->label, "ota_0") == 0) {
    update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
    Serial.println("Mise à jour vers partition OTA_1");
    use_ota_api = true;
  } else if (strcmp(running->label, "ota_1") == 0) {
    update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    Serial.println("Mise à jour vers partition OTA_0");
    use_ota_api = true;
  }
  
  // Si on utilise les partitions OTA, utiliser l'API OTA native
  if (use_ota_api && update_partition != NULL) {
    esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
      Serial.printf("Erreur esp_ota_begin: %s\n", esp_err_to_name(err));
      http.end();
      return false;
    }
    Serial.println("Partition OTA initialisée avec succès");
  } else {
    // Fallback: utiliser Update.begin() avec U_FLASH
    Serial.println("Utilisation de Update.begin() (fallback)");
    if (!Update.begin(contentLength, U_FLASH)) {
      Serial.printf("Erreur Update.begin: %s\n", Update.errorString());
      http.end();
      return false;
    }
  }

  // initialiser SHA256
  mbedtls_sha256_context sha_ctx;
  mbedtls_sha256_init(&sha_ctx);
  mbedtls_sha256_starts(&sha_ctx, 0); // 0 = SHA-256

  WiFiClient *stream = http.getStreamPtr();
  uint8_t buff[128];
  int remaining = contentLength;
  Serial.printf("Taille: %d\n", contentLength);
  while (http.connected() && (remaining > 0 || contentLength == -1)) {
    size_t sizeAvailable = stream->available();
    if (sizeAvailable) {
      int toRead = stream->readBytes(buff, ((sizeAvailable > (int)sizeof(buff)) ? sizeof(buff) : sizeAvailable));
      // update mbedtls sha
      mbedtls_sha256_update(&sha_ctx, buff, toRead);
      
      // write to flash (utiliser l'API appropriée)
      esp_err_t write_err = ESP_OK;
      if (use_ota_api && ota_handle != 0) {
        write_err = esp_ota_write(ota_handle, buff, toRead);
      } else {
        if (Update.write(buff, toRead) != toRead) {
          write_err = ESP_FAIL;
        }
      }
      
      if (write_err != ESP_OK) {
        Serial.println("Erreur écriture flash");
        mbedtls_sha256_free(&sha_ctx);
        if (use_ota_api && ota_handle != 0) {
          esp_ota_abort(ota_handle);
        }
        http.end();
        return false;
      }
      if (contentLength != -1) remaining -= toRead;
    } else {
      delay(10);
    }
  }

  // finalize sha
  unsigned char sha_result[32];
  mbedtls_sha256_finish(&sha_ctx, sha_result);
  mbedtls_sha256_free(&sha_ctx);

  // convert sha_result to hex (en minuscules)
  char sha_hex[65];
  for (int i = 0; i < 32; i++) sprintf(sha_hex + i*2, "%02x", sha_result[i]);
  sha_hex[64] = 0;
  
  // Normaliser le SHA attendu : enlever les espaces, convertir en minuscules
  char expected_sha_normalized[65];
  int j = 0;
  for (int i = 0; expectedSha256Hex[i] != '\0' && i < 128 && j < 64; i++) {
    char c = expectedSha256Hex[i];
    if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
      expected_sha_normalized[j++] = tolower(c);
    }
  }
  expected_sha_normalized[j] = '\0';
  
  Serial.printf("SHA256 local: %s\n", sha_hex);
  Serial.printf("SHA256 attendu (original): %s\n", expectedSha256Hex);
  Serial.printf("SHA256 attendu (normalisé): %s\n", expected_sha_normalized);
  Serial.printf("Longueur local: %d, attendu: %d\n", strlen(sha_hex), strlen(expected_sha_normalized));

  http.end();

  // comparer (les deux sont maintenant en minuscules et sans espaces)
  if (strcmp(sha_hex, expected_sha_normalized) != 0) {
    Serial.println("SHA mismatch ! Annulation.");
    Serial.printf("Comparaison: local[0-10]=%.10s, attendu[0-10]=%.10s\n", sha_hex, expected_sha_normalized);
    if (use_ota_api && ota_handle != 0) {
      esp_ota_abort(ota_handle);
    } else {
      Update.end(false); // Annuler l'update en cas d'erreur
    }
    return false;
  }
  
  Serial.println("SHA256 vérifié avec succès !");

  // Finaliser l'update selon l'API utilisée
  esp_err_t finalize_err = ESP_OK;
  if (use_ota_api && ota_handle != 0) {
    finalize_err = esp_ota_end(ota_handle);
    if (finalize_err != ESP_OK) {
      Serial.printf("Erreur esp_ota_end: %s\n", esp_err_to_name(finalize_err));
      return false;
    }
    
    // Définir la partition de boot
    if (esp_ota_set_boot_partition(update_partition) == ESP_OK) {
      Serial.printf("Partition de boot définie: %s\n", update_partition->label);
      // Réinitialiser le compteur de boot pour la nouvelle version
      preferences.putInt(PREF_BOOT_COUNT, 0);
      preferences.putBool(PREF_BOOT_SUCCESS, false);
      Serial.println("Update écrite avec succès !");
      Serial.println("Redémarrage dans 2 secondes...");
      delay(2000);
      ESP.restart(); // Redémarrer l'ESP32
      return true; // Ne sera jamais atteint, mais nécessaire pour la compilation
    } else {
      Serial.println("ERREUR: Impossible de définir la partition de boot !");
      return false;
    }
  } else {
    // Utiliser Update.end() pour le fallback
    if (Update.end()) {
      if (Update.isFinished()) {
        Serial.println("Update écrite avec succès !");
        Serial.println("Redémarrage dans 2 secondes...");
        delay(2000);
        ESP.restart(); // Redémarrer l'ESP32
        return true; // Ne sera jamais atteint, mais nécessaire pour la compilation
      } else {
        Serial.println("Update non terminée correctement.");
        return false;
      }
    } else {
      Serial.printf("Update.end() erreur #%d\n", Update.getError());
      return false;
    }
  }
}

void checkUpdate() {
  Serial.println("Vérification update...");
  WiFiClientSecure client;
  client.setCACert(root_ca);

  String jsonStr = httpGetStringSecure(updateJsonUrl, client);
  if (jsonStr.length() == 0) {
    Serial.println("Impossible de récupérer update.json");
    Serial.printf("URL: %s\n", updateJsonUrl);
    return;
  }
  
  Serial.printf("JSON reçu (%d caractères): %s\n", jsonStr.length(), jsonStr.substring(0, 200).c_str());

  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, jsonStr);
  if (err) {
    Serial.println("Erreur parse JSON");
    Serial.println(err.c_str());
    return;
  }

  String newVersion = doc["version"].as<String>();
  String firmwareUrl = doc["url"].as<String>();
  String sha256 = doc["sha256"].as<String>();

  Serial.printf("Version distante: %s\n", newVersion.c_str());
  Serial.printf("URL firmware (avant correction): %s\n", firmwareUrl.c_str());
  
  // Corriger l'URL GitHub si nécessaire
  firmwareUrl = fixGitHubUrl(firmwareUrl);
  
  Serial.printf("URL firmware (après correction): %s\n", firmwareUrl.c_str());
  Serial.printf("SHA256 attendu: %s\n", sha256.c_str());
  
  if (newVersion != currentVersion) {
    Serial.println("Nouvelle version disponible -> téléchargement");
    bool ok = downloadAndUpdate(firmwareUrl.c_str(), sha256.c_str());
    if (ok) {
      // Le redémarrage est déjà géré dans downloadAndUpdate()
      // Ne pas redémarrer ici pour éviter un double redémarrage
    } else {
      Serial.println("Update échouée.");
    }
  } else {
    Serial.println("Pas de mise à jour (version identique).");
  }
}

// Configuration du serveur web
void setupWebServer() {
  // Endpoint pour forcer une vérification de mise à jour
  server.on("/update", HTTP_GET, []() {
    Serial.println("Requête reçue: /update - Vérification forcée de mise à jour");
    forceUpdateCheck = true;
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Vérification de mise à jour déclenchée\"}");
  });
  
  // Endpoint pour recevoir des webhooks GitHub
  server.on("/webhook", HTTP_POST, []() {
    Serial.println("\n=== Webhook GitHub reçu ===");
    
    // Récupérer le payload selon le Content-Type
    String contentType = server.header("Content-Type");
    String payload = "";
    
    if (contentType.indexOf("application/json") != -1) {
      // Format JSON
      payload = server.arg("plain");
      Serial.println("Format: JSON");
    } else if (contentType.indexOf("application/x-www-form-urlencoded") != -1) {
      // Format form-urlencoded (format par défaut GitHub)
      payload = server.arg("payload");
      Serial.println("Format: x-www-form-urlencoded");
    } else {
      // Essayer les deux formats
      payload = server.arg("plain");
      if (payload.length() == 0) {
        payload = server.arg("payload");
      }
      Serial.println("Format: Auto-détecté");
    }
    
    Serial.printf("Payload reçu (%d caractères)\n", payload.length());
    
    // Parser le JSON du webhook (optionnel, pour extraire des infos)
    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, payload);
    
    if (!err) {
      // Vérifier si c'est un événement de push ou release
      String ref = doc["ref"].as<String>();
      String action = doc["action"].as<String>();
      
      Serial.printf("Événement GitHub: ref=%s, action=%s\n", ref.c_str(), action.c_str());
      
      // Si c'est un push sur main/master ou une release, déclencher la mise à jour
      if (ref.indexOf("main") != -1 || ref.indexOf("master") != -1 || action == "published") {
        Serial.println(">>> Déclenchement de la mise à jour suite au webhook...");
        forceUpdateCheck = true;
        server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Mise à jour déclenchée\"}");
      } else {
        Serial.println("Événement ignoré (pas un push sur main/master)");
        server.send(200, "application/json", "{\"status\":\"ignored\",\"message\":\"Événement ignoré\"}");
      }
    } else {
      // Même sans JSON valide, déclencher la vérification (pour compatibilité)
      Serial.println("Webhook reçu (format non-JSON ou JSON invalide)");
      Serial.println("Déclenchement de la vérification par sécurité...");
      forceUpdateCheck = true;
      server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Vérification déclenchée\"}");
    }
    
    Serial.println("=== Fin du traitement du webhook ===\n");
  });
  
  // Endpoint pour le statut du système
  server.on("/status", HTTP_GET, []() {
    const esp_partition_t* running = esp_ota_get_running_partition();
    int bootCount = preferences.getInt(PREF_BOOT_COUNT, 0);
    bool bootSuccess = preferences.getBool(PREF_BOOT_SUCCESS, false);
    
    String status = "{";
    status += "\"version\":\"" + currentVersion + "\",";
    status += "\"partition\":\"" + String(running->label) + "\",";
    status += "\"boot_count\":" + String(bootCount) + ",";
    status += "\"boot_success\":" + String(bootSuccess ? "true" : "false") + ",";
    status += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    status += "\"wifi_status\":" + String(WiFi.status() == WL_CONNECTED ? "\"connected\"" : "\"disconnected\"");
    status += "}";
    
    server.send(200, "application/json", status);
  });
  
  // Endpoint racine
  server.on("/", HTTP_GET, []() {
    String html = "<!DOCTYPE html><html><head><title>ESP32 OTA System</title></head><body>";
    html += "<h1>ESP32 OTA System</h1>";
    html += "<p>Version actuelle: <strong>" + currentVersion + "</strong></p>";
    html += "<p>IP: <strong>" + WiFi.localIP().toString() + "</strong></p>";
    html += "<h2>Actions:</h2>";
    html += "<ul>";
    html += "<li><a href=\"/update\">Forcer une vérification de mise à jour</a></li>";
    html += "<li><a href=\"/status\">Voir le statut (JSON)</a></li>";
    html += "</ul>";
    html += "<h2>Webhook GitHub:</h2>";
    html += "<p><strong>Payload URL:</strong> <code>http://" + WiFi.localIP().toString() + "/webhook</code></p>";
    html += "<p><strong>Content type:</strong> <code>application/x-www-form-urlencoded</code> ou <code>application/json</code></p>";
    html += "<p><strong>Méthode:</strong> <code>POST</code></p>";
    html += "<p><strong>Événements:</strong> Just the push event (recommandé)</p>";
    html += "<h3>Instructions:</h3>";
    html += "<ol>";
    html += "<li>Allez dans votre repo GitHub → Settings → Webhooks</li>";
    html += "<li>Cliquez sur 'Add webhook'</li>";
    html += "<li>Collez l'URL ci-dessus dans 'Payload URL'</li>";
    html += "<li>Sélectionnez 'Just the push event'</li>";
    html += "<li>Cliquez sur 'Add webhook'</li>";
    html += "</ol>";
    html += "<p><em>Note: Si votre ESP32 n'est pas accessible depuis Internet, utilisez un service de tunneling (ngrok, localtunnel) et utilisez l'URL fournie par ce service.</em></p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });
  
  server.begin();
}

void loop() {
  // Gérer les requêtes du serveur web
  server.handleClient();
  
  // Vérifier les mises à jour UNIQUEMENT si déclenchée manuellement ou via webhook
  if (WiFi.status() == WL_CONNECTED && forceUpdateCheck) {
    Serial.println("=== Déclenchement de la vérification de mise à jour ===");
    forceUpdateCheck = false;
    checkUpdate();
  }
  
  delay(100); // Petit délai pour éviter de surcharger le CPU
}


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>
#include "logger.h"
#include "config.h"


const ButtonConfig* getButtonConfig(int pin) {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (BUTTON_CONFIGS[i].buttonPin == pin) {
      return &BUTTON_CONFIGS[i];
    }
  }
  return NULL;
}

RTC_DATA_ATTR bool hasInitializedTime = false;

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// bool syncNTPTime() {
//   Serial.print("Syncing time with NTP server");
//   configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  
//   unsigned long startAttemptTime = millis();
  
//   while (millis() - startAttemptTime < NTP_TIMEOUT) {
//     time_t now = time(nullptr);
//     if (now > 8 * 3600 * 2) {
//       Serial.println("\nTime synced!");
//       return true;
//     }
//     delay(500);
//     Serial.print(".");
//   }
  
//   Serial.println("\nTime sync failed!");
//   return false;
// }

String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01T00:00:00Z";
  }
  
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buffer);
}

// bool connectToWiFi() {
//   Serial.print("Connecting to WiFi");
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
//   unsigned long startAttemptTime = millis();
  
//   while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
//     delay(500);
//     Serial.print(".");
//   }
  
//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\nWiFi connected!");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
//     return true;
//   } else {
//     Serial.println("\nWiFi connection failed!");
//     return false;
//   }
// }

// bool connectToMQTT() {
//   espClient.setCACert(ROOT_CA);
//   espClient.setInsecure();
//   Serial.print("Connecting to MQTT broker");
//   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
//   unsigned long startAttemptTime = millis();
  
//   while (!mqttClient.connected() && millis() - startAttemptTime < MQTT_TIMEOUT) {
//     if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
//       Serial.println("\nMQTT connected!");
//       return true;
//     }
//     delay(500);
//     Serial.print(".");
//   }
  
//   Serial.println("\nMQTT connection failed!");
//   return false;
// }

void goToSleep() {
  Serial.println("Going to deep sleep...");
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  Serial.flush();
  
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(60000000); // måske 1 minut
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_CONFIGS[i].buttonPin, INPUT);
    pinMode(BUTTON_CONFIGS[i].ledPin, OUTPUT);
    digitalWrite(BUTTON_CONFIGS[i].ledPin, LOW);
  }
  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER || !hasInitializedTime || ESP_SLEEP_WAKEUP_EXT1){
    initLogger();
  }

  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER || !hasInitializedTime) {
    Serial.println("Woke up from timer for sync!");
    
    if (connectToWiFi()) {
      if (syncNTPTime()) {
        hasInitializedTime = true;
      }
      
      if (connectToMQTT()) {
        syncLoggedEvents(mqttClient, MQTT_TOPIC);
        mqttClient.disconnect();
      }
    }
  }
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    Serial.println("Woke up from button press!");
        
    uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
    int wakeup_pin = __builtin_ctzll(wakeup_pin_mask);
    
    const ButtonConfig* config = getButtonConfig(wakeup_pin);
    
    if (config) {
      digitalWrite(config->ledPin, HIGH);
      unsigned long ledOnTime = millis();
      
      Serial.print(config->mood);
      Serial.print(" smiley selected! ");
      Serial.println(config->emoji);
      
      logButtonPress(config->buttonNumber, config->emoji, config->mood);
      
      unsigned long elapsedTime = millis() - ledOnTime;
      if (elapsedTime < LED_DISPLAY_TIME) {
        delay(LED_DISPLAY_TIME - elapsedTime);
      }
      
      digitalWrite(config->ledPin, LOW);
    }
  }
  
  goToSleep();
}

void loop() {
}

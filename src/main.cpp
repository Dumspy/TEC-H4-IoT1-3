#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include "logger.h"

const char* WIFI_SSID = "IoT_H3/4";
const char* WIFI_PASSWORD = "98806829";

const char* MQTT_SERVER = "wilson.local";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "esp32/felix/buttons";
const char* MQTT_CLIENT_ID = "ESP32_Felix_Button_Device";

const int WIFI_TIMEOUT = 10000;
const int MQTT_TIMEOUT = 5000;

const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 3600;
const int DAYLIGHT_OFFSET_SEC = 3600;
const int NTP_TIMEOUT = 10000;

struct ButtonConfig {
  int buttonPin;
  int ledPin;
  int buttonNumber;
  const char* emoji;
  const char* mood;
};

static const ButtonConfig BUTTON_CONFIGS[] = {
  {26, 18, 1, "😡", "angry"},
  {25, 4, 2, "☹️", "sad"},
  {33, 2, 3, "😐", "neutral"},
  {32, 15, 4, "😊", "happy"},
};

const int BUTTON_COUNT = sizeof(BUTTON_CONFIGS) / sizeof(BUTTON_CONFIGS[0]);

const ButtonConfig* getButtonConfig(int pin) {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (BUTTON_CONFIGS[i].buttonPin == pin) {
      return &BUTTON_CONFIGS[i];
    }
  }
  return NULL;
}

const int LED_DISPLAY_TIME = 7000;

#define BUTTON_PIN_BITMASK ((1ULL << 25) | (1ULL << 32) | (1ULL << 33) | (1ULL << 26))

RTC_DATA_ATTR time_t lastNTPSync = 0;
RTC_DATA_ATTR bool hasInitializedTime = false;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool syncNTPTime() {
  Serial.print("Syncing time with NTP server");
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  
  unsigned long startAttemptTime = millis();
  
  while (millis() - startAttemptTime < NTP_TIMEOUT) {
    time_t now = time(nullptr);
    if (now > 8 * 3600 * 2) {
      Serial.println("\nTime synced!");
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nTime sync failed!");
  return false;
}

String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01T00:00:00Z";
  }
  
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buffer);
}

bool connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nWiFi connection failed!");
    return false;
  }
}

bool connectToMQTT() {
  Serial.print("Connecting to MQTT broker");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  unsigned long startAttemptTime = millis();
  
  while (!mqttClient.connected() && millis() - startAttemptTime < MQTT_TIMEOUT) {
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("\nMQTT connected!");
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nMQTT connection failed!");
  return false;
}

void goToSleep() {
  Serial.println("Going to deep sleep...");
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  Serial.flush();
  
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(3600 * 1000000); // 1 hour
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

  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER || !hasInitializedTime) {
    Serial.println("Woke up from timer for sync!");

    initLogger();
    
    if (connectToWiFi()) {
      if (syncNTPTime()) {
        lastNTPSync = time(nullptr);
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
    
    initLogger();
    
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

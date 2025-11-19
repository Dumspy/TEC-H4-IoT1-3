#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>
#include "logger.h"

const char* WIFI_SSID = "IoT_H3/4";
const char* WIFI_PASSWORD = "98806829";

const char* MQTT_SERVER = "wilson.local";
const int MQTT_PORT = 8883;
const char* MQTT_TOPIC = "esp32/mood/buttons";
const char* MQTT_CLIENT_ID = "ESP32_Mood_Tracker_Device";

const int WIFI_TIMEOUT = 10000;
const int MQTT_TIMEOUT = 5000;

const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 3600;
const int DAYLIGHT_OFFSET_SEC = 3600;
const int NTP_TIMEOUT = 10000;

const char* user = "elev1";
const char* password = "password";


const char* ROOT_CA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDBTCCAe2gAwIBAgIUXopiLMN2tHLH5ALoHzUf4tY5upUwDQYJKoZIhvcNAQEL\n" \
"BQAwEjEQMA4GA1UEAwwHTVFUVC1DQTAeFw0yNTExMTYwOTQyMDlaFw0zNTExMTQw\n" \
"OTQyMDlaMBIxEDAOBgNVBAMMB01RVFQtQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
"DwAwggEKAoIBAQDTJehrHOy4YN3OmnVumeYq7gA7vd8iUoBLCsiHIGLAfnWA8b0f\n" \
"r9XK3T552ztwfnZ6uK+T8kpqdQL0OczNXyEz2oi5e5MJAXYr1ytXns1SUQVgzj8F\n" \
"Igl9i7MuwEYLyj9VjIjQgAhqDL2LlNomGyCfYoIhxJ0mvOii+yNsD5ghM2uLpaMu\n" \
"FoP6k00gl0T/2fnJYKmy2frsRYxQLScf+arMHyXKcK89DVy5jQpFNhXHzqKwFgTY\n" \
"kl0VIWp11ZKjCI3CNWl2t5U/GHbdHGf94TtXPvG5c6oe4hvd9On6d0fm12K5kc0C\n" \
"ceQCq5pXP/z+a4acH8s2Iqfs+h0v8i4DtkFZAgMBAAGjUzBRMB0GA1UdDgQWBBTz\n" \
"lQPbwuJ9LBR524FJXVYQArXcPjAfBgNVHSMEGDAWgBTzlQPbwuJ9LBR524FJXVYQ\n" \
"ArXcPjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQALGWCrskwS\n" \
"BfaPnFZs8pm/FAcehS/YvdClyHoL4XBg7EqW+f72SiBy6Nu0Hgj7tPBwC0tPpKHA\n" \
"IwVp5UN54IYGXnIgXirZOSxGBnoqS1ttrRYvrngmXg8woiQzaKryVEOGzwEI9/MI\n" \
"qaclcDnK0G7KbYRnJ2o5mneyY87OKyBznW9ceas7o4hzGvKojY67nGrayWwjO7Iw\n" \
"/4cXqqfWhE1x7znsU4IUXaL6CN/cpe6+forBUi84Cv6uVmeRhsABt+ENxaLG6CDu\n" \
"Fd0rtKYmoqbkfqPGXeRVfvbJg0PJmrBHZG2KOMDC37fvIysHGc+jYl6yWh6b5mEG\n" \
"uOn/9KgYpDzd\n" \
"-----END CERTIFICATE-----\n";

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

RTC_DATA_ATTR bool hasInitializedTime = false;

WiFiClientSecure espClient;
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
  espClient.setCACert(ROOT_CA);
  Serial.print("Connecting to MQTT broker");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  unsigned long startAttemptTime = millis();
  
  while (!mqttClient.connected() && millis() - startAttemptTime < MQTT_TIMEOUT) {
    if (mqttClient.connect(MQTT_CLIENT_ID, user, password)) {
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

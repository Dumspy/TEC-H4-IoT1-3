#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "IoT_H3/4";
const char* WIFI_PASSWORD = "98806829";

const char* MQTT_SERVER = "wilson.local";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "esp32/felix/buttons";
const char* MQTT_CLIENT_ID = "ESP32_Felix_Button_Device";

const int WIFI_TIMEOUT = 10000;
const int MQTT_TIMEOUT = 5000;

const int BUTTON_PIN_1 = 25;
const int BUTTON_PIN_2 = 32;
const int BUTTON_PIN_3 = 33;
const int BUTTON_PIN_4 = 26;

const int LED_PIN_1 = 4;
const int LED_PIN_2 = 15;
const int LED_PIN_3 = 2;
const int LED_PIN_4 = 18;

const int LED_DISPLAY_TIME = 3000;

#define BUTTON_PIN_BITMASK ((1ULL << 25) | (1ULL << 32) | (1ULL << 33) | (1ULL << 26))

WiFiClient espClient;
PubSubClient mqttClient(espClient);

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

void publishButtonPress(int buttonNumber, const char* emoji, const char* mood) {
  if (mqttClient.connected()) {
    char message[100];
    snprintf(message, sizeof(message), "{\"button\":%d,\"mood\":\"%s\",\"emoji\":\"%s\"}", buttonNumber, mood, emoji);
    
    if (mqttClient.publish(MQTT_TOPIC, message)) {
      Serial.print("Published: ");
      Serial.println(message);
    } else {
      Serial.println("Publish failed!");
    }
    
    mqttClient.loop();
    delay(100);
  }
}

void goToSleep() {
  Serial.println("Going to deep sleep...");
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  Serial.flush();
  
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(9600);
  delay(100);
  
  pinMode(BUTTON_PIN_1, INPUT);
  pinMode(BUTTON_PIN_2, INPUT);
  pinMode(BUTTON_PIN_3, INPUT);
  pinMode(BUTTON_PIN_4, INPUT);
  
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    Serial.println("Woke up from button press!");
    
    uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
    
    int buttonNumber = 0;
    const char* emoji = "";
    const char* mood = "";
    
    if (wakeup_pin_mask & (1ULL << BUTTON_PIN_1)) {
      digitalWrite(LED_PIN_1, HIGH);
      buttonNumber = 1;
      emoji = "☹️";
      mood = "sad";
      Serial.println("Sad smiley selected! ☹️");
    }
    else if (wakeup_pin_mask & (1ULL << BUTTON_PIN_2)) {
      digitalWrite(LED_PIN_2, HIGH);
      buttonNumber = 2;
      emoji = "😊";
      mood = "happy";
      Serial.println("Happy smiley selected! 😊");
    }
    else if (wakeup_pin_mask & (1ULL << BUTTON_PIN_3)) {
      digitalWrite(LED_PIN_3, HIGH);
      buttonNumber = 3;
      emoji = "😐";
      mood = "neutral";
      Serial.println("Neutral smiley selected! 😐");
    }
    else if (wakeup_pin_mask & (1ULL << BUTTON_PIN_4)) {
      digitalWrite(LED_PIN_4, HIGH);
      buttonNumber = 4;
      emoji = "😡";
      mood = "angry";
      Serial.println("Angry smiley selected! 😡");
    }
    
    if (connectToWiFi()) {
      if (connectToMQTT()) {
        publishButtonPress(buttonNumber, emoji, mood);
        mqttClient.disconnect();
      }
    }
    
    delay(LED_DISPLAY_TIME);
    
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
    digitalWrite(LED_PIN_4, LOW);
  } else {
    Serial.println("Smiley System Initialized!");
    Serial.println("Button 1 (GPIO 25) = Sad ☹️");
    Serial.println("Button 2 (GPIO 32) = Happy 😊");
    Serial.println("Button 3 (GPIO 33) = Neutral 😐");
    Serial.println("Button 4 (GPIO 26) = Angry 😡");
  }
  
  goToSleep();
}

void loop() {
}

#include <Arduino.h>

const int BUTTON_PIN_1 = 25;
const int BUTTON_PIN_2 = 32;
const int BUTTON_PIN_3 = 33;

const int LED_PIN_1 = 4;
const int LED_PIN_2 = 15;
const int LED_PIN_3 = 2;

const int LED_DISPLAY_TIME = 3000;

#define BUTTON_PIN_BITMASK ((1ULL << 25) | (1ULL << 32) | (1ULL << 33))

void goToSleep() {
  Serial.println("Going to deep sleep...");
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
  
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    Serial.println("Woke up from button press!");
    
    uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
    
    if (wakeup_pin_mask & (1ULL << BUTTON_PIN_1)) {
      digitalWrite(LED_PIN_1, HIGH);
      Serial.println("Happy smiley selected! 😊");
    }
    else if (wakeup_pin_mask & (1ULL << BUTTON_PIN_2)) {
      digitalWrite(LED_PIN_2, HIGH);
      Serial.println("Neutral smiley selected! 😐");
    }
    else if (wakeup_pin_mask & (1ULL << BUTTON_PIN_3)) {
      digitalWrite(LED_PIN_3, HIGH);
      Serial.println("Sad smiley selected! ☹️");
    }
    
    delay(LED_DISPLAY_TIME);
    
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
  } else {
    Serial.println("Smiley System Initialized!");
    Serial.println("Button 1 (GPIO 25) = Happy 😊");
    Serial.println("Button 2 (GPIO 32) = Neutral 😐");
    Serial.println("Button 3 (GPIO 33) = Sad ☹️");
  }
  
  goToSleep();
}

void loop() {
}

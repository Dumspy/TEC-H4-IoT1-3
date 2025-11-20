#include "buttonHandler.h"

ButtonHandler::ButtonHandler()
{
	  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_CONFIGS[i].buttonPin, INPUT);
    pinMode(BUTTON_CONFIGS[i].ledPin, OUTPUT);
    digitalWrite(BUTTON_CONFIGS[i].ledPin, LOW);
  }
}

const ButtonConfig* ButtonHandler::getPressedButtonConfig()
{
  uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
  int wakeup_pin = __builtin_ctzll(wakeup_pin_mask);
    
  const ButtonConfig* config = getButtonConfig(wakeup_pin);
  
  return config;
}

void ButtonHandler::provideFeedBack(const ButtonConfig* config) 
{
    if (!config) return;
    
    digitalWrite(config->ledPin, HIGH);
    unsigned long ledOnTime = millis();
    
    Serial.print(config->mood);
    Serial.print(" smiley selected! ");
    Serial.println(config->emoji);
    
    unsigned long elapsedTime = millis() - ledOnTime;
    if (elapsedTime < LED_DISPLAY_TIME) {
        delay(LED_DISPLAY_TIME - elapsedTime);
    }
    
    digitalWrite(config->ledPin, LOW);
}

void ButtonHandler::enableDeepSleepWakeup()
{
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
}

const ButtonConfig* ButtonHandler::getButtonConfig(int pin)
{
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (BUTTON_CONFIGS[i].buttonPin == pin) {
      return &BUTTON_CONFIGS[i];
    }
  }
  return NULL;
}
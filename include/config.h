#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Wifi config
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const int WIFI_TIMEOUT;

// MQTT config
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const char* MQTT_TOPIC;
extern const char* MQTT_CLIENT_ID;
extern const int MQTT_TIMEOUT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

// NTP config
extern const char* NTP_SERVER;
extern const long GMT_OFFSET_SEC;
extern const int DAYLIGHT_OFFSET_SEC;
extern const int NTP_TIMEOUT;

// Certificate
extern const char* ROOT_CA;

struct ButtonConfig {
    int buttonPin;
    int ledPin;
    int buttonNumber;
    const char* emoji;
    const char* mood;
};

// Array med button og LED config
static const ButtonConfig BUTTON_CONFIGS[] = {
    {26, 18, 1, "😡", "angry"},
    {25, 4, 2, "☹️", "sad"},
    {33, 2, 3, "😐", "neutral"},
    {32, 15, 4, "😊", "happy"},
};

extern const int BUTTON_COUNT;
extern const int LED_DISPLAY_TIME;

// Deep Sleep Wakeup Mask
#define BUTTON_PIN_BITMASK ((1ULL << 25) | (1ULL << 32) | (1ULL << 33) | (1ULL << 26))

#endif
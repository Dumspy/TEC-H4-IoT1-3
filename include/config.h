#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Wifi config
const char* WIFI_SSID = "IoT_H3/4";
const char* WIFI_PASSWORD = "98806829";
const int WIFI_TIMEOUT = 10000;

// MQTT config
const char* MQTT_SERVER = "wilson.local";
const int MQTT_PORT = 8883;
const char* MQTT_TOPIC = "esp32/mood/buttons";
const char* MQTT_CLIENT_ID = "ESP32_Mood_Tracker_Device";
const int MQTT_TIMEOUT = 5000;
const char* MQTT_USER = "elev1";
const char* MQTT_PASSWORD = "password";

// NTP config
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 3600;
const int DAYLIGHT_OFFSET_SEC = 3600;
const int NTP_TIMEOUT = 10000;

// Certificate
const char* ROOT_CA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDBTCCAe2gAwIBAgIUXopiLMN2tHLH5ALoHzUf4tY5upUwDQYJKoZIhvcNAQELBQAwEjEQMA4GA1UEAwwHTVFUVC1DQTAeFw0yNTExMTYwOTQyMDlaFw0zNTExMTQwOTQyMDlaMBIxEDAOBgNVBAMMB01RVFQtQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDTJehrHOy4YN3OmnVumeYq7gA7vd8iUoBLCsiHIGLAfnWA8b0fr9XK3T552ztwfnZ6uK+T8kpqdQL0OczNXyEz2oi5e5MJAXYr1ytXns1SUQVgzj8FIgl9i7MuwEYLyj9VjIjQgAhqDL2LlNomGyCfYoIhxJ0mvOii+yNsD5ghM2uLpaMuFoP6k00gl0T/2fnJYKmy2frsRYxQLScf+arMHyXKcK89DVy5jQpFNhXHzqKwFgTYkl0VIWp11ZKjCI3CNWl2t5U/GHbdHGf94TtXPvG5c6oe4hvd9On6d0fm12K5kc0CceQCq5pXP/z+a4acH8s2Iqfs+h0v8i4DtkFZAgMBAAGjUzBRMB0GA1UdDgQWBBTzlQPbwuJ9LBR524FJXVYQArXcPjAfBgNVHSMEGDAWgBTzlQPbwuJ9LBR524FJXVYQArXcPjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQALGWCrskwSBfaPnFZs8pm/FAcehS/YvdClyHoL4XBg7EqW+f72SiBy6Nu0Hgj7tPBwC0tPpKHAIwVp5UN54IYGXnIgXirZOSxGBnoqS1ttrRYvrngmXg8woiQzaKryVEOGzwEI9/MIqaclcDnK0G7KbYRnJ2o5mneyY87OKyBznW9ceas7o4hzGvKojY67nGrayWwjO7Iw/4cXqqfWhE1x7znsU4IUXaL6CN/cpe6+forBUi84Cv6uVmeRhsABt+ENxaLG6CDuFd0rtKYmoqbkfqPGXeRVfvbJg0PJmrBHZG2KOMDC37fvIysHGc+jYl6yWh6b5mEGuOn/9KgYpDzd" \
"\n-----END CERTIFICATE-----";

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

const int BUTTON_COUNT = sizeof(BUTTON_CONFIGS) / sizeof(BUTTON_CONFIGS[0]);
const int LED_DISPLAY_TIME = 7000;

// Deep Sleep Wakeup Mask
#define BUTTON_PIN_BITMASK ((1ULL << 25) | (1ULL << 32) | (1ULL << 33) | (1ULL << 26))

#endif
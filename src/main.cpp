#include "logger.h"
#include "config.h"
#include "buttonHandler.h"
#include "connectionHandler.h"

// Wifi config
const char* WIFI_SSID = "TEC-IOT";
const char* WIFI_PASSWORD = "42090793";
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

const char* user = "elev1";
const char* password = "password";

const char* ROOT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDBTCCAe2gAwIBAgIUXopiLMN2tHLH5ALoHzUf4tY5upUwDQYJKoZIhvcNAQEL
BQAwEjEQMA4GA1UEAwwHTVFUVC1DQTAeFw0yNTExMTYwOTQyMDlaFw0zNTExMTQw
OTQyMDlaMBIxEDAOBgNVBAMMB01RVFQtQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IB
DwAwggEKAoIBAQDTJehrHOy4YN3OmnVumeYq7gA7vd8iUoBLCsiHIGLAfnWA8b0f
r9XK3T552ztwfnZ6uK+T8kpqdQL0OczNXyEz2oi5e5MJAXYr1ytXns1SUQVgzj8F
Igl9i7MuwEYLyj9VjIjQgAhqDL2LlNomGyCfYoIhxJ0mvOii+yNsD5ghM2uLpaMu
FoP6k00gl0T/2fnJYKmy2frsRYxQLScf+arMHyXKcK89DVy5jQpFNhXHzqKwFgTY
kl0VIWp11ZKjCI3CNWl2t5U/GHbdHGf94TtXPvG5c6oe4hvd9On6d0fm12K5kc0C
ceQCq5pXP/z+a4acH8s2Iqfs+h0v8i4DtkFZAgMBAAGjUzBRMB0GA1UdDgQWBBTz
lQPbwuJ9LBR524FJXVYQArXcPjAfBgNVHSMEGDAWgBTzlQPbwuJ9LBR524FJXVYQ
ArXcPjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQALGWCrskwS
BfaPnFZs8pm/FAcehS/YvdClyHoL4XBg7EqW+f72SiBy6Nu0Hgj7tPBwC0tPpKHA
IwVp5UN54IYGXnIgXirZOSxGBnoqS1ttrRYvrngmXg8woiQzaKryVEOGzwEI9/MI
qaclcDnK0G7KbYRnJ2o5mneyY87OKyBznW9ceas7o4hzGvKojY67nGrayWwjO7Iw
/4cXqqfWhE1x7znsU4IUXaL6CN/cpe6+forBUi84Cv6uVmeRhsABt+ENxaLG6CDu
Fd0rtKYmoqbkfqPGXeRVfvbJg0PJmrBHZG2KOMDC37fvIysHGc+jYl6yWh6b5mEG
uOn/9KgYpDzd
-----END CERTIFICATE-----
)EOF";

const int BUTTON_COUNT = sizeof(BUTTON_CONFIGS) / sizeof(BUTTON_CONFIGS[0]);
const int LED_DISPLAY_TIME = 7000;


RTC_DATA_ATTR bool hasInitializedTime = false;

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
ConnectionHandler connHandler(espClient, mqttClient);
ButtonHandler btnHandler;

String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01T00:00:00Z";
  }
  
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buffer);
}

void goToSleep() {
  Serial.println("Going to deep sleep...");
  
  connHandler.disconnectWiFi();
  
  Serial.flush();
  
  btnHandler.enableDeepSleepWakeup();
  esp_sleep_enable_timer_wakeup(60000000); // måske 1 minut
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(9600);
  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER || !hasInitializedTime || ESP_SLEEP_WAKEUP_EXT1){
    initLogger();
  }

  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER || !hasInitializedTime) {
    Serial.println("Woke up from timer for sync!");
    
    if (connHandler.connectToWiFi()) {
      if (connHandler.syncNTPTime()) {
        hasInitializedTime = true;
      }
      
      if (connHandler.connectToMQTT()) {
        syncLoggedEvents(mqttClient, MQTT_TOPIC);
        connHandler.getMqttClient().disconnect();
      }
    }
  }
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    Serial.println("Woke up from button press!");
    
    const ButtonConfig* config = btnHandler.getPressedButtonConfig();
    
    if (config)
    {
			btnHandler.provideFeedBack(config);
			logButtonPress(config->buttonNumber, config->emoji, config->mood);
    }
  }
  
  goToSleep();
}

void loop() {
}

#include "connectionHandler.h"
#include "config.h"
#include <time.h>

ConnectionHandler::ConnectionHandler(WiFiClientSecure& client, PubSubClient& mqtt) : espClient(client), mqttClient(mqtt) {}

bool ConnectionHandler::connectoToWiFi()
{
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

bool ConnectionHandler::syncNTPTime()
{
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

bool ConnectionHandler::connectToMQTT()
{
  espClient.setCACert(ROOT_CA);
  espClient.setInsecure();
  Serial.print("Connecting to MQTT broker");
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  unsigned long startAttemptTime = millis();
  
  while (!mqttClient.connected() && millis() - startAttemptTime < MQTT_TIMEOUT) {
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("\nMQTT connected!");
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nMQTT connection failed!");
  return false;
}

void ConnectionHandler::disconnectWiFi()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

PubSubClient& ConnectionHandler::getMqttClient(){
	return mqttClient;	
}
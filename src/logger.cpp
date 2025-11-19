#include <Arduino.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include "logger.h"

const char* LOG_FILE = "/button_log.txt";

extern String getFormattedTime();

void initLogger() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }
  Serial.println("LittleFS mounted successfully");
}

void logButtonPress(int buttonNumber, const char* emoji, const char* mood) {
  File file = LittleFS.open(LOG_FILE, "a");
  if (!file) {
    Serial.println("Failed to open log file for writing");
    return;
  }
  
  String timestamp = getFormattedTime();
  char logEntry[150];
  snprintf(logEntry, sizeof(logEntry), "{\"button\":%d,\"mood\":\"%s\",\"emoji\":\"%s\",\"timestamp\":\"%s\"}\n", buttonNumber, mood, emoji, timestamp.c_str());
  
  file.print(logEntry);
  file.close();
  
  Serial.print("Logged: ");
  Serial.println(logEntry);
}

int syncLoggedEvents(PubSubClient& mqttClient, const char* mqttTopic) {
  if (!LittleFS.exists(LOG_FILE)) {
    Serial.println("No log file to sync");
    return 0;
  }
  
  File file = LittleFS.open(LOG_FILE, "r");
  if (!file) {
    Serial.println("Failed to open log file for reading");
    return 0;
  }
  
  int eventCount = 0;
  bool allPublished = true;
  
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    
    if (line.length() > 0) {
      eventCount++;
      
      if (mqttClient.connected()) {
        if (mqttClient.publish(mqttTopic, line.c_str())) {
          Serial.print("Published: ");
          Serial.println(line);
        } else {
          Serial.println("Publish failed!");
          allPublished = false;
        }
        mqttClient.loop();
        delay(100);
      } else {
        allPublished = false;
      }
    }
  }
  
  file.close();
  
  if (allPublished && eventCount > 0) {
    LittleFS.remove(LOG_FILE);
    Serial.print("Synced and cleared ");
    Serial.print(eventCount);
    Serial.println(" events");
  } else if (!allPublished) {
    Serial.println("Some events failed to publish, keeping log file");
  }
  
  return eventCount;
}

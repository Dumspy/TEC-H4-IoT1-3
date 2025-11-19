#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <PubSubClient.h>

void initLogger();
void logButtonPress(int buttonNumber, const char* emoji, const char* mood);
int syncLoggedEvents(PubSubClient& mqttClient, const char* mqttTopic);

#endif

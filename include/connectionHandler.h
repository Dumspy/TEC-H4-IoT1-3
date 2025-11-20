#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include "WiFi.h"
#include "WiFiClientSecure.h"
#include <PubSubClient.h>

class ConnectionHandler
{
private:
	WiFiClientSecure& espClient;
	PubSubClient& mqttClient;

public:
	// '&' efter typen betyder at det er en reference, til et eksisterende objekt, der bliver passed med og ikke en ny instans af objektet
	ConnectionHandler(WiFiClientSecure& client, PubSubClient& mqtt);
	
	bool connectToWiFi();
	bool syncNTPTime();
	bool connectToMQTT();
	void disconnectWiFi();
	
	PubSubClient& getMqttClient();
};

#endif
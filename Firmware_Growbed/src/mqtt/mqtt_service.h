#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "time/time_control.h"

#define PINLUZ D2
#define PINVENTILADOR D3

extern const char* mqttServer;
extern const int mqttPort;
extern bool message_arrived;
extern PubSubClient client;
extern const char* mqttUser;
extern const char* mqttPassword;
extern String messageInTopic;
extern const char* topicIncome;

extern String GrowbedTopic;
extern String controlZona;
extern String controlInvernadero;
extern String controlCama;
extern String aliveTopic;
extern bool flagMessagesInArray;
extern int countMessagesInArray;

extern char GROWBED_TOPIC [60];
extern char CONTROL_ZONE [60];
extern char CONTROL_GREENHOUSE [60];
extern char CONTROL_GROWBED [60];
extern char ALIVE_TOPIC [60];

extern const int zone;
extern const int greenhouse;
extern const int growbed;

extern void mqtt_init();
extern void mqtt_conect();
extern void publishInTopic(const char* topic, const char* message);
extern void publishDataFormat(const char *topic, String message);
extern void jsonProcess(String topicMessage);
extern void buildTopicsNames();
extern void tryBrokerReconnect();
extern void sendMessagesInArray();

#endif
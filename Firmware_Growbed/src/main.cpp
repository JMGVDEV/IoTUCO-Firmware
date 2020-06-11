#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <mqtt/mqtt_service.h>
#include <time/time_control.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN D5
#define PINLUZ D2
#define PINVENTILADOR D3

String GrowbedTopic;
String controlZona;
String controlInvernadero;
String controlCama;
String aliveTopic;

char GROWBED_TOPIC [60];
char CONTROL_ZONE [60];
char CONTROL_GREENHOUSE [60];
char CONTROL_GROWBED [60];
char ALIVE_TOPIC [60];

const char* ssid = "xxx";
const char* password =  "xxx";
const char* mqttServer = "xxx";
//const char* mqttServer = "xx";
const int mqttPort = 	xxx;
const char* mqttUser = "xxx";
const char* mqttPassword = "xxx";

const int zone = 1;
const int greenhouse = 1;
const int growbed = 1;
const int sensorReadPeriod = 10;
DHT dht;

const long utcOffsetInSeconds = -18000;

WiFiClient mqttCli;
PubSubClient client(mqttCli);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

String dataGrowbed;
String device_id;

bool flagMessagesInArray = false;
bool flagReconnected = true;
int countMessagesInArray = 0;
int arrayNotSended[1024][3];

void setup_wifi() {
  
  delay(10);
  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void setup() {
  
  Serial.begin(115200);
  delay(10);

  //Connect to WiFi
  setup_wifi();

  //Build topics names
  buildTopicsNames();

  //Connect to MQTT Broker
  mqtt_init();
 
  //MQTT Connection Validation
  mqtt_conect();

  //NTP client connection
  timeClient.begin();
  timeClient.update();
  sec_Ant = timeToNextRead();

  //Initilize humidity sensor
  dht.setup(DHTPIN);

  pinMode(PINLUZ,OUTPUT);
  pinMode(PINVENTILADOR,OUTPUT);
  
}

void loop() {
  client.loop();

  //Antes de actualizar la hora desde el servidor, se verifica que exista
  //conexión wifi para que el micro no se quede bloqueado.
  if(WiFi.status() == WL_CONNECTED){
    timeClient.update();
  }

  sec_Act = timeClient.getSeconds();

  if(IsTimeToRead(sec_Act)){
    
    Serial.println(WiFi.status());
    dataGrowbed = ReadSensor();

    if((WiFi.status() == WL_CONNECTED) && (client.connected())){
      publishDataFormat(GROWBED_TOPIC, dataGrowbed);
      Serial.println(dataGrowbed);
    }else{
      flagMessagesInArray = true;
      Serial.println("problemas con la conexión WIFI");
      addElement();
      tryBrokerReconnect();
    }

  }

  if (message_arrived)
  {
    message_arrived = false;
    jsonProcess(messageInTopic);
    Serial.println("Message arrive");
    Serial.println(messageInTopic);
  }

}
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <mqtt/mqtt_service.h>
#include <time/time_control.h>
#include <DHT.h>


#define DHTPIN D5
#define magneticSensor D7
#define PINPUERTA D2
#define PINCORTINA D3

String DoorTopic;
String EnviromentTopic;
String controlInvernadero;
String controlZona;
String aliveTopic;

char DOOR_TOPIC [60];
char ENVIROMENT_TOPIC [60];
char CONTROL_GREENHOUSE [60];
char CONTROL_ZONE [60];
char ALIVE_TOPIC [60];

const char* ssid = "xx";
const char* password =  "xxx";
const char* mqttServer = "xxx";
//const char* mqttServer = "xx";
const int mqttPort = 	xx;
const char* mqttUser = "xx";
const char* mqttPassword = "xx";

const int zone = 1;
const int greenhouse = 1;
const int sensorReadPeriod = 10;

DHT dht;

const long utcOffsetInSeconds = -18000;

WiFiClient mqttCli;
PubSubClient client(mqttCli);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

String dataInv;
String device_id;
int doorStateAnt = HIGH;
int doorStateAct = HIGH;

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

  //build device_id name
  //device_id = deviceIdName();

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

  pinMode(PINPUERTA,OUTPUT);
  pinMode(PINCORTINA,OUTPUT);
  pinMode(magneticSensor, INPUT_PULLUP);
  doorStateAnt = digitalRead(magneticSensor);

}

void loop() {
  client.loop();
  timeClient.update();
  sec_Act = timeClient.getSeconds();
  doorStateAct = digitalRead(magneticSensor);

  if(IsTimeToRead(sec_Act)){
    
    dataInv = ReadSensor();
    publishDataFormat(ENVIROMENT_TOPIC, dataInv);
    Serial.println(dataInv);
  }

  if(doorStateAct != doorStateAnt){
    doorInterrupt(doorStateAct);
    doorStateAnt = doorStateAct;
  }

  if (message_arrived)
  {
    message_arrived = false;
    jsonProcess(messageInTopic);
    Serial.print("Message arrive: ");
    Serial.println(messageInTopic);
  }
}

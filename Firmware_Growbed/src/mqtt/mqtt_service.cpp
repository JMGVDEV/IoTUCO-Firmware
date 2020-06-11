#include "mqtt_service.h"

bool message_arrived = false;
String messageInTopic = "";
const char* topicIncome = "";

enum peripheral
{
  luz,
  ventilador,
  puerta,
  cortinas
};

/********* MQTT Callback ***************************
   here is defined the logic to execute after
   a messages arrives in the desired
   topic, for this example the variable:
   timePetition
************************************************/

void callback(char* topic, byte* payload, unsigned int length) {

  message_arrived = true;
  messageInTopic="";

  //Notify about message arrived 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  topicIncome=topic;

  for (int i = 0; i < length; i++) {
    messageInTopic += (char)payload[i];
  }
}

void mqtt_init(){
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}


void mqtt_conect(){
  while (!client.connected() && WiFi.status() == WL_CONNECTED) {
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
 
      Serial.println("Broker connected");
      client.subscribe(CONTROL_GREENHOUSE);
      client.subscribe(CONTROL_GROWBED);
      client.subscribe(CONTROL_ZONE);
      publishDataFormat(ALIVE_TOPIC, aliveMessage()); 
       
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
}

void tryBrokerReconnect(){
  if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
    Serial.println("Broker reconnected");
    if(flagMessagesInArray){
      sendMessagesInArray();
    }
    
  }else{
    Serial.print("failed with state ");
    Serial.println(client.state());
    
  }
}

void sendMessagesInArray(){
  int i = 0;

  while(i < countMessagesInArray){

    publishDataFormat(GROWBED_TOPIC, buildMessageStored(arrayNotSended[i][0], arrayNotSended[i][1], arrayNotSended[i][2]));
    arrayNotSended[i][0] = 0;
    arrayNotSended[i][1] = 0;
    arrayNotSended[i][2] = 0;
    i++;
    delay(1000);

  }
  
  sec_Ant = timeToNextRead();
  flagMessagesInArray = false;
  countMessagesInArray = 0;

}

void buildTopicsNames(){
  
//"medicion/zona/<zone>/invernadero/<greenhuose>/cama/<growbed>/ambiente"
  GrowbedTopic = "medicion/zona/";
  GrowbedTopic += (String)zone;
  GrowbedTopic += "/invernadero/";
  GrowbedTopic += (String)greenhouse;
  GrowbedTopic += "/cama/";
  GrowbedTopic += (String)growbed;
  GrowbedTopic += "/ambiente";
  GrowbedTopic.toCharArray(GROWBED_TOPIC, 60);

//"medicion/zona/<zone>/invernadero/<greenhuose>/cama/<growbed>"
  controlCama = "control/zona/";
  controlCama += (String)zone;
  controlCama += "/invernadero/";
  controlCama += (String)greenhouse;
  controlCama += "/cama/";
  controlCama += (String)growbed;
  controlCama.toCharArray(CONTROL_GROWBED, 60);

//"control/zona/<zone>/invernadero/<greenhuose>"
  controlInvernadero = "control/zona/";
  controlInvernadero += (String)zone;
  controlInvernadero += "/invernadero/";
  controlInvernadero += (String)greenhouse;
  controlInvernadero.toCharArray(CONTROL_GREENHOUSE, 60);

//"control/zona/<zone>"
  controlZona = "control/zona/";
  controlZona += (String)zone;
  controlZona.toCharArray(CONTROL_ZONE, 60);

//"zona/<zone>/invernadero/<greenhuose>/cama/<growbed>/alive"
  aliveTopic = "zona/";
  aliveTopic += (String)zone;
  aliveTopic += "/invernadero/";
  aliveTopic += (String)greenhouse;
  aliveTopic += "/cama/";
  aliveTopic += (String)growbed;
  aliveTopic += "/alive";
  aliveTopic.toCharArray(ALIVE_TOPIC, 60);
}

void publishInTopic(const char *topic, const char* message){

    /*if (client.state() < 0)
    {
      tryBrokerReconnect();
    }*/

    client.publish(topic, message);
}

void publishDataFormat(const char *topic, String message){

    /*if (client.state() < 0)
    {
      tryBrokerReconnect();
    }*/

    char data[100];
    message.toCharArray(data, 100);
    client.publish(topic, data);
    Serial.print("Published in: ");
    Serial.println(topic);
}

void jsonProcess(String topicMessage){
    StaticJsonDocument<94> doc;
    char messageChar[94];
    messageInTopic.toCharArray(messageChar, 94);
    deserializeJson(doc, messageChar);

    int periferico = doc["peripheral"];
    int valor = doc["value"];
    
    if(periferico == luz){
      digitalWrite(PINLUZ, valor);
    }else if(periferico == ventilador){
      digitalWrite(PINVENTILADOR, valor);
    }else{
      return;
    }

}
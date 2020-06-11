#include "time_control.h"

int sec_Ant = 0;
int sec_Act = 0;
char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sábado"};
float humidity = 0.0;
float temperature = 0.0;
ulong epochTime = 0;

int timeToNextRead(){
    sec_Ant = timeClient.getSeconds() + sensorReadPeriod;

    if (sec_Ant >= 60)
    {
        sec_Ant = sec_Ant - 60; 
    }
    return sec_Ant;
}

bool IsTimeToRead(int sec_act){
    if(sec_act == sec_Ant){
        timeToNextRead();

        return true;
    }else{
        return false;
    }
}

String ReadSensor(){
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    epochTime = timeClient.getEpochTime();

    String dataSensor;
    StaticJsonDocument<120> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["zone"] = zone;
    doc["greenhouse"] = greenhouse;
    doc["growbed"] = growbed;
    doc["hour"] = epochTime;
    serializeJson(doc, dataSensor);

    return dataSensor;
}

String buildMessageStored (int temperature, int humidity, int epochTime){
    String data;
    StaticJsonDocument<120> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["zone"] = zone;
    doc["greenhouse"] = greenhouse;
    doc["growbed"] = growbed;
    doc["hour"] = epochTime;
    serializeJson(doc, data);

    return data;
}

void addElement(){
    countMessagesInArray++;
    arrayNotSended[countMessagesInArray][0] = (int)(temperature);
    arrayNotSended[countMessagesInArray][1] = (int)(humidity);
    arrayNotSended[countMessagesInArray][2] = (int)epochTime;
    Serial.print("array: ");
    Serial.println(countMessagesInArray);
}

String deviceIdName(){

    String deviceName = (String)zone;
    deviceName += "_";
    deviceName += (String)greenhouse;
    deviceName += "_";
    deviceName += (String)growbed;

    return deviceName;
}

String aliveMessage(){
    String message;
    StaticJsonDocument<100> doc;
    doc["zone"] = zone;
    doc["greenhouse"] = greenhouse;
    doc["growbed"] = growbed;
    serializeJson(doc, message);

    return message;
}

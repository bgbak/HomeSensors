
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

//
// WIFI and MQTT setup
//
#define CLIENT_NAME "Bedroom"
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP,BROKER_PORT,wifiClient);

//
// Sensor setup
//

#define DHTPIN 2
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);

#define SECONDS_BETWEEN_MEASUREMENTS 60
unsigned long lastTime = 0;
bool firstTime = true;
char msg[50];

void connectToWiFiAndBroker() 
{
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WIFI!");

  Serial.println("Connecting to broker");
  while (!mqttClient.connect(CLIENT_NAME)) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to broker!");
}


char *ftoa(char *buffer, float f)
{ 
  char *returnString = buffer;
  long integerPart = (long)f;
  itoa(integerPart, buffer, 10);
  while (*buffer != '\0') buffer++;
  *buffer++ = '.';
  long decimalPart = abs((long)((f - integerPart) * 100));
  itoa(decimalPart, buffer, 10);
  return returnString;
}

void publishFloatValue(float value, char* topic)
{
    if (isnan(value)) 
    {
      Serial.println("Invalid value!");
      return;
    }

    Serial.println("Publishing a new value");
    ftoa(msg, value);
    Serial.println(msg);
    mqttClient.publish(topic, msg);
}

void setup() 
{
  Serial.begin(9600);
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  dht.begin();
}


void loop() 
{ 

  if ( firstTime || (millis() - lastTime > SECONDS_BETWEEN_MEASUREMENTS*1000) ) 
  {
    firstTime = false;
    lastTime = millis();
  
    if (!mqttClient.connected()) 
    {
      connectToWiFiAndBroker();
    }

    mqttClient.loop();

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    }
    else {
      Serial.print("Temperature: ");
      Serial.print(event.temperature);
      Serial.println(" *C");
      float temperature = event.temperature;
      publishFloatValue(temperature,"Home/Bedroom/Temperature");
    }
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    }
    else {
      Serial.print("Humidity: ");
      Serial.print(event.relative_humidity);
      Serial.println("%");
      float humidity = event.relative_humidity;
      publishFloatValue(humidity,"Home/Bedroom/Humidity");
    }
  }
}

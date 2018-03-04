
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
#define CLIENT_NAME "Office"
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP, BROKER_PORT, wifiClient);

//
// Sensor setup
//

#define DHTPIN 2
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);

#define SECONDS_BETWEEN_MEASUREMENTS 60
const int sleepSeconds = 10;
unsigned long lastTime = 0;
bool firstTime = true;
char msg[50];

void connectToWifi()
{
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WIFI!");
}

void connectToBroker()
{
  Serial.println("Connecting to broker");
  while (!mqttClient.connect(CLIENT_NAME))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to broker!");
}

char *ftoa(char *buffer, float f)
{
  char *returnString = buffer;
  long integerPart = (long)f;
  itoa(integerPart, buffer, 10);
  while (*buffer != '\0')
    buffer++;
  *buffer++ = '.';
  long decimalPart = abs((long)((f - integerPart) * 100));
  itoa(decimalPart, buffer, 10);
  return returnString;
}

void publishFloatValue(float value, char *topic)
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
  //Initialize serial connection
  Serial.begin(9600);

  //Set WiFi mode to Standard
  WiFi.mode(WIFI_STA);

  //Start connecting to the AP
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  //Initialize DHT sensor
  dht.begin();
}

void loop()
{

  // Test if we are connected to the AP
  if (!WiFi.isConnected())
  {
    connectToWifi();
  }

  // Test if we are connected to the MQTT broker
  if (!mqttClient.connected())
  {
    connectToBroker();
  }

  if (firstTime || (millis() - lastTime > SECONDS_BETWEEN_MEASUREMENTS * 1000))
  {

    firstTime = false;
    lastTime = millis(); 

    mqttClient.loop();

    //Initialize event class
    sensors_event_t event;

    //Get temperature event from sensor.
    dht.temperature().getEvent(&event);

    //Check if we actually got a value
    if (isnan(event.temperature))
    {
      Serial.println("Error reading temperature!");
      mqttClient.publish("Home/Office/Errors", "Error reading temp");

    }
    //We got a value!
    else
    {
      Serial.print("Temperature: ");
      Serial.print(event.temperature);
      Serial.println(" *C");
      float temperature = event.temperature;
      //Quick! Send it to the MQTT Broker!
      publishFloatValue(temperature, "Home/Office/Temperature");
    }

    //Get humidity event from sensor
    dht.humidity().getEvent(&event);

    //Check if we actually got a value
    if (isnan(event.relative_humidity))
    {
      Serial.println("Error reading humidity!");
    }
    //We got a value!
    else
    {
      Serial.print("Humidity: ");
      Serial.print(event.relative_humidity);
      Serial.println("%");
      float humidity = event.relative_humidity;
      //Quick! Send it to the MQTT Broker!
      publishFloatValue(humidity, "Home/Office/Humidity");
    }
  }
}

# HomeSensors

A little project using Wemos D1 with DHT and battery shields to monitor temperature and humidity around the house.

Devices send measurement data to an MQTT broker.  
A small python script subscribes to the topics, and stores measurement values in InfluxDB.  
Grafana uses the InfluxDB as datasource.  

### Python requirements
* paho-mqtt
* influxdb


### Note: Deep sleep
The sketch uses deep sleep to save battery.
Connect Pin D0 to RST on the WeMos. Otherwise the device will never wake up.

Thanks to Lars Bergqvist for inspiration and code. larsbergqvist.wordpress.com
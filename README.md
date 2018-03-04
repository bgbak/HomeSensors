#HomeSensors

A little project using Wemos D1 with DHT and battery shields to monitor temperature and humidity around the house.

#
Devices send measurement data to an MQTT broker.
A small python script subscribes to the topics, and stores measurement values in InfluxDB.
Grafana uses the InfluxDB as datasource.


**Thanks**
Thanks to Lars Bergqvist for inspiration and code. larsbergqvist.wordpress.com
/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/kathrinburtscher/Particle/Particle_Temp_Hum_To_AWS_Azure_Device_Cloud/src/Particle_Temp_Hum_To_AWS_Azure_Device_Cloud.ino"
/*
 * Project Particle_Temp_Hum_To_AWS_Azure_Device_Cloud
 * Description:
 * Author:
 * Date:
 */

#include "MQTT-TLS.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"

//AWS MQTT
void setup();
void loop();
#line 12 "/Users/kathrinburtscher/Particle/Particle_Temp_Hum_To_AWS_Azure_Device_Cloud/src/Particle_Temp_Hum_To_AWS_Azure_Device_Cloud.ino"
const char awsRootCert[] = "-----BEGIN CERTIFICATE-----\r\nMIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n********\r\nrqXRfboQnoZsG4q5WTP468SQvvG5\r\n-----END CERTIFICATE-----";
const char awsClientCert[] = "-----BEGIN CERTIFICATE-----\r\nMIIDWTCCAkGgAwIBAgIUN94dGYvsTJoeQvwxz6JgJcdOWjUwDQYJKoZIhvcNAQEL\r\n*******\r\nIdGvOolrrjKBhaGyutj4VsS3LZU8hraii+rQyfD5pJ7M8qQ+0ODV37b8Nb9Y\r\n-----END CERTIFICATE-----";
const char awsClientKey[] = "-----BEGIN RSA PRIVATE KEY-----\r\nMIIEowIBAAKCAQEA2dS+/HsIGCN6d0tz+BZQ8eNj18OiyjNBzevQEdJ4tRKcfXgk\r\r******\r\nJnrlUw37EZMHI6EFF8AaVHRGshoOge8X9Xg8WJelwTkTZo38lZzU\r\n-----END RSA PRIVATE KEY-----";

void callback(char *topic, byte *payload, unsigned int length);

//Create MQTT client with AWS IoT-Core endpoint as host
MQTT client("******************.iot.eu-central-1.amazonaws.com", 8883, callback);

void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
}

//Temperature and Humidity Sensor
DHT sensor(D2);
String temp;
String hum;

//Control the delay over the Particle app by publishing the changeDelaySeconds method
long int delaySeconds = 10;
int changeDelaySeconds(String args);

// setup() runs once, when the device is first turned on.
void setup()
{
  //Make the variable and method refering to the seconds of delay public in the Particle app
  Particle.variable("delaySeconds", delaySeconds);
  Particle.function("changeDelaySeconds", changeDelaySeconds);

  //AWS
  client.enableTls(awsRootCert, sizeof(awsRootCert), awsClientCert, sizeof(awsClientCert), awsClientKey, sizeof(awsClientKey));
  client.connect("Particle_1");
  client.publish("sensor/test", "temp");

  //Temperature and Humidity Sensor
  Serial.begin(9600);
  sensor.begin();
}

// loop() runs over and over again (delay duration can be manipulated in the Particle App)
void loop()
{

  //get sensor data
  temp = String(sensor.getTempCelcius());
  hum = String(sensor.getHumidity());

  //AWS
  if (client.isConnected())
  {
    client.loop();
    client.publish("sensor/temperature", temp);
  }

  //AZURE build data string and publish data
  String res = "{\"temperature\":" + temp + ", \"humidity\":" + hum + "}";
  Particle.publish("temperature", res, PRIVATE);

  //Particle webhook Thingspeak
  Particle.publish("thingspeak", temp, PRIVATE);

  //wait for delaySeconds seconds
  delay(delaySeconds * 1000);
}

//Public method to change the delay duration
int changeDelaySeconds(String args)
{
  delaySeconds = args.toInt();
  return 1;
}
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "sensor.h"

// define an esp32:pub/sub topic
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
float h, t = 0.0;

// define a function protoype for the message handler
void messageHandler(char* topic, byte* payload, unsigned int length);

void connectAWS(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Connecting to WiFi...");
  while(WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use AWS IoT device credentials
  net.setCACert(AWS_IOT_ROOT_CA_1);
  net.setCertificate(AWS_IOT_CERTIFICATE);
  net.setPrivateKey(AWS_IOT_PRIVATE_KEY);

  // establish a connection to the MQTT broker on the AWS endpoint.
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  //create a message hander
  client.setCallback(messageHandler);
  Serial.println("Connecting to AWS IoT ...");

  while(!client.connect(THINGNAME)){
    Serial.print(".");
    delay(50);
  } if (!client.connected()){
    Serial.println("AWS IoT connection failed");
    return;
  }
  // subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT connection established");
}

void publishMessage(){
  StaticJsonDocument<200> doc; // create a JSON object, of capacity 200 bytes (the default) 
  doc["humidity"] = h;
  doc["temperature"] = t;
  char jsonBuffer[512]; // create a buffer to hold the JSON object
  serializeJson(doc, jsonBuffer); // serialize the JSON object to the buffer;
                                  // prints to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);                                  
}

void messageHandler(char* topic, byte* payload, unsigned int length){
  Serial.println("Incoming message: ");
  Serial.println(topic);
  
  StaticJsonDocument<200> doc; // create a JSON object
  deserializeJson(doc, payload); // deserialize the JSON object from the buffer
  const char* message = doc["message"]; // get the message from the JSON object
  Serial.println(message);
}

void setup(){
  Serial.begin(115200);
  connectAWS(); // connect to AWS IoT 
  sensorSetup(); // setup sensor
  Serial.println("Setup complete");
}

void loop(){
  while (1)
  {
    sensorLoop(); // get sensor data
    publishMessage(); // publish sensor data to AWS IoT
    client.loop(); // check for messages
    delay(1000);
  }
}
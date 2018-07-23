
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 5000; 

char ssid[] = "NETGEAR26"; // your network SSID (name)
char pass[] = "12345678"; // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's status
int a =0;
// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial Serial1(10,11); // RX, TX   //ESP8266 RX,TX connected to these pins
void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
 
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  //connect to MQTT server
  client.setServer("192.168.1.24", 1883);
  client.setCallback(callback);
  
  startMillis = millis();
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);

  Serial.print("] ");
  for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    if (receivedChar == '0')
      Serial.println("Off");
    if (receivedChar == '1')
      Serial.println("On");

  }
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }

  
  String str=String(a);
  //Serial.println(str);
  char msg[20];
  
  str.toCharArray(msg,20);
  //Serial.println(msg);
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    publishing(msg);
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
  
    client.loop();
}

void publishing(char* msg){
  a++;
  client.publish("esp/test", msg);
  Serial.print("Sent ");
  Serial.print(msg);
  Serial.print(" at ");
  Serial.println(currentMillis);

}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("UNO","pi","codexpress")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("esp/test","Hello World from ESP!");
      // ... and resubscribe
      //client.subscribe("esp/test",0);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

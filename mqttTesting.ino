#include <PubSubClient.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include <SoftwareSerial.h>
#define RX 10
#define TX 11
char mqttUserName[] = "CodeXpress";  // Can be any name.
char mqttPass[] = " VWA0R335S3O3USS1";  // Change this your MQTT API Key from Account > MyProfile.
char writeAPIKey[] = "ZE1X91RSHEB4YXD5";    // Change to your channel Write API Key.
long channelID = 523307;
char ssid[] = "NETGEAR26";           // your network SSID (name)
char pass[] = "12345678";           // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status
const char* server = "mqtt.thingspeak.com"; 
int clientID[8];

// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(RX,TX); // RX, TX
void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  soft.begin(115200);
  // initialize ESP module
  WiFi.init(&soft);

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
  client.setServer(server, 1883);
  client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }else{
    Serial.println("Success");
  }
  client.loop();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("command","hello world");
      // ... and resubscribe
      client.subscribe("presence");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
   /* while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(12345678,mqttUserName,mqttPass)) 
    {
      Serial.println("connected");
    } else 
    {
      Serial.print("failed, rc=");
      // Print to know why the connection failed.
      // See http://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    } 
    */

  }


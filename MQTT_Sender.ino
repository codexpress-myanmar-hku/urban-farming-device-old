#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>



char ssid[] = "NETGEAR26"; // your network SSID (name)
char pass[] = "12345678"; // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's status

// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial Serial1(10,11); // RX, TX   //ESP8266 RX,TX connected to these pins
void setup() {
	// initialize serial for debugging
	Serial.begin(115200);
	// initialize serial for ESP module
	Serial1.begin(115200);
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
	client.setServer("192.168.1.137", 1883);
	client.setCallback(callback);
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
	client.loop();
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
			client.subscribe("esp/test",0);

		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

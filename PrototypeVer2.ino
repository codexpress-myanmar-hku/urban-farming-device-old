
//setup Wifi and mqtt
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
//include libraries for soil temperature
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>//library for dht11(air temperature and humidity)
#include <SoftwareSerial.h>//library for esp8266
#define ONE_WIRE_BUS 12
#define RX 10
#define TX 11
#define DHT11PIN 9
#define soil_sign A0
#define VT_PIN A1 
#define AT_PIN A2
dht DHT11;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//Define variable for timing
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 10000; 

//settings for wifi connection to thingspeak
/*String AP = "NETGEAR26";       // CHANGE ME
String PASS = "12345678"; // CHANGE ME*/
int a =0;
char ssid[] = "NETGEAR26"; // your network SSID (name)
char pass[] = "12345678"; // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's status
String API = "ZE1X91RSHEB4YXD5";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
//String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial Serial1(RX,TX); 
WiFiEspClient espClient;
PubSubClient client(espClient);
 
void setup()
{
   Serial.begin(9600); 
   Serial1.begin(9600);
   //AT command
   /*sendCommand("AT",5,"OK");
   sendCommand("AT+CWMODE=1",5,"OK");
   sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");*/

   //espwifi
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

    //sensors setting
    //pinMode(relay_pin,OUTPUT);
    pinMode(soil_sign, INPUT); 
    sensors.begin();//begin the soil temperature sensors
}

void loop()
{
  //reconnect when disconnect
  if (!client.connected()) {
    reconnect();
  }
  
  //sensors values
  int sensorValue = analogRead(soil_sign); 
  int vt_read = analogRead(VT_PIN);
  int at_read = analogRead(AT_PIN);
  int chk = DHT11.read11(DHT11PIN);
  int tem=(int)DHT11.temperature;
  int hum=(int)DHT11.humidity;
  float voltage = vt_read * (5.0 / 1024.0) * 5.0;
  float current = at_read * (5.0 / 1024.0);
  float watts = voltage * current;
    
  sensors.requestTemperatures();
  Serial.print("Soil temperature: ");
  Serial.println(sensors.getTempCByIndex(0));
  Serial.print("Moisture: ");
  Serial.println(sensorValue);
  Serial.print("DHT Temperature: ");
  Serial.println(tem);
  Serial.print("DHT Humidity: ");
  Serial.println(hum);
  Serial.print("Volts: "); 
  Serial.print(voltage, 3);
  Serial.print("\tAmps: ");
  Serial.print(current,3);
  Serial.print("\tWatts: ");
  Serial.println(watts,3);
  Serial.println();

  String str=String(tem)+", "+String(hum)+", "+String(sensorValue)+", "+String(sensors.getTempCByIndex(0));
  Serial.println(str);
  char msg[50];
  
  str.toCharArray(msg,50);
  //timing
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    publishing(msg);
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }

  
//sending data to thingspeak.
  senddata(tem,"field1");
  senddata(hum,"field2");
  senddata(sensorValue,"field3");
  senddata(sensors.getTempCByIndex(0),"field4");
  sendCommand("AT+CIPSTATUS",8,"STATUS");
  sendCommand("AT+CIPCLOSE",5,"OK");

  
  
    client.loop();
 }



void senddata(int data, String field){
  Serial.print("sending data:");
  Serial.println(data);
  valSensor =data ;
  String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
  sendCommand("AT+CIPMUX=1",5,"OK");
  bool sent=0;
  int retried=0;
  while((!sent) &&(retried<5)){       //retry when fail
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sent=sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
    Serial1.println(getData);
    retried++;
  }
  delay(1500);countTrueCommand++;
}


int sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  found = false;
  while(countTimeCommand < (maxTime*1))
  {
    Serial1.println(command);//at+cipsend
    if(Serial1.find(readReplay))//ok
    {
      found = true;      
      break;
    }
    countTimeCommand++;
  }
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
    return 1;
  } 
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
    return 0;
  }  
 }


 void publishing(char* msg){
  a++;
  client.publish("esp/test", msg);
  Serial.print(a);
  Serial.print(". Sent ");
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

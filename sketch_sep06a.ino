#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>//library for dht11(air temperature and humidity)
#include <SoftwareSerial.h>//library for esp8266
#include <LiquidCrystal.h>//library for lcd screen
#define ONE_WIRE_BUS 12
#define RX 10
#define TX 11
#define DHT11PIN 9
dht DHT11;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//settings for all sensors
//int relay_pin=8;
int soil_sign = A0; 
int soil_val = 900; 
int tem_val=26;
//settings for wifi connection to thingspeak
String AP = "NETGEAR26";       // CHANGE ME
String PASS = "12345678"; // CHANGE ME
String API = "ZE1X91RSHEB4YXD5";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
int a=0;
SoftwareSerial esp8266(RX,TX);  
 
void setup()
{
   Serial.begin(9600); 
   esp8266.begin(115200);
   //AT command
   sendCommand("AT",5,"OK");
   sendCommand("AT+CWMODE=1",5,"OK");
   sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
   //sensors setting
   //pinMode(relay_pin,OUTPUT);
   pinMode(soil_sign, INPUT); 
   sensors.begin();//begin the soil temperature sensors
}

void loop()
{
  //sensors values
  int chk = DHT11.read11(DHT11PIN);
  int tem=(int)DHT11.temperature;
  int hum=(int)DHT11.humidity;
  int sensorValue = analogRead(soil_sign);   
  sensors.requestTemperatures();
  Serial.print("Soil temperature: ");
  Serial.println(sensors.getTempCByIndex(0));
  Serial.print("Moisture: ");
  Serial.println(sensorValue);
  Serial.print("DHT Temperature: ");
  Serial.println(tem);
  Serial.print("DHT Humidity: ");
  Serial.println(hum);
  
  
//sending data to thingspeak(be modified later). currently just send testing data to my own site. Plz set up thingspeak fast.
  //wificonnect();
  valSensor =a ;
  a+=10;
  String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);delay(1500);countTrueCommand++;
  //Serial.println(a);
  sendCommand("AT+CIPCLOSE",5,"OK");
  delay(100);
  /*if((analogRead(soil_sign) >soil_val))
  {
    digitalWrite(relay_pin,HIGH);
  delay(3000);
      digitalWrite(relay_pin,LOW); //LOW to poweroff the LED light on jidianqi
  }*/
  delay(2000);
 }

 void wificonnect(){
  valSensor =a ;
  a+=10;
  String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);delay(1500);countTrueCommand++;
  Serial.println(a);
  sendCommand("AT+CIPCLOSE",5,"OK");
  delay(100);
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
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
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }


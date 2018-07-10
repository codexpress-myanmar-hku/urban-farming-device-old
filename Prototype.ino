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
//settings for wifi connection to thingspeak
String AP = "NETGEAR26";       // CHANGE ME
String PASS = "12345678"; // CHANGE ME
String API = "ZE1X91RSHEB4YXD5";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
//String field = "field1";
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


  
//sending data to thingspeak.
  senddata(tem,"field1");
  senddata(hum,"field2");
  senddata(sensorValue,"field3");
  senddata(sensors.getTempCByIndex(0),"field4");
  sendCommand("AT+CIPSTATUS",8,"STATUS");
  sendCommand("AT+CIPCLOSE",5,"OK");
  delay(2000);
 }



void senddata(int data, String field){
  Serial.print("sending data:");
  Serial.println(data);
  valSensor =data ;
  String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
  sendCommand("AT+CIPMUX=1",5,"OK");
  bool sent=0;
  while(!sent){       //retry when fail
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sent=sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
    esp8266.println(getData);
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


//include libraries for soil temperature
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
int relay_pin=8;
int soil_sign = A0; 
int soil_val = 900; 
int tem_val=26;
//settings for wifi connection to thingspeak
String AP = "NETGEAR26";       // CHANGE ME
String PASS = "12345678"; // CHANGE ME
String API = "ZE1X91RSHEB4YXD5";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
int a=0;
SoftwareSerial esp8266(RX,TX); 
LiquidCrystal lcd(7,6,5,4,3,2); 
 
void setup()
{
   Serial.begin(9600); 
   esp8266.begin(115200);
   //AT command
   sendCommand("AT",5,"OK");
   sendCommand("AT+CWMODE=1",5,"OK");
   sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
   //sensors setting
   pinMode(relay_pin,OUTPUT);
   pinMode(soil_sign, INPUT); 
   lcd.begin(16,2); //设置LCD显示的数目。16 X 2：16格2行。
   lcd.print("Watering System"); 
   sensors.begin();//begin the soil temperature sensors
}

void loop()
{
  //sensors values
  int chk = DHT11.read(DHT11PIN);
  int tem=(int)DHT11.temperature;
  int hum=(int)DHT11.humidity;
  int sensorValue = analogRead(soil_sign);   
  sensors.requestTemperatures();
  Serial.println(sensors.getTempCByIndex(0));
  //lcd screen
  lcd.clear();
  lcd.print("Watering System"); //
  lcd.setCursor(0,1);  //将闪烁的光标设置到column 0, line 1 (注释：从0开始数起，line 0是显示第一行，line 1是第二行。)
  lcd.print("S:"); //开机后屏幕现实以秒几时的时间
  lcd.setCursor(2,1);
  lcd.print(sensorValue); 
  lcd.setCursor(7,1);
  lcd.print("T:");
  lcd.setCursor(9,1);
  lcd.print(tem); 
  lcd.setCursor(12,1);
  lcd.print("H:");
  lcd.setCursor(14,1);
  lcd.print(hum);
  
//sending data to thingspeak(be modified later). currently just send testing data to my own site. Plz set up thingspeak fast.
  //wificonnect();
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
  if((analogRead(soil_sign) >soil_val))
  {
    digitalWrite(relay_pin,HIGH);
  delay(3000);
      digitalWrite(relay_pin,LOW); //LOW to poweroff the LED light on jidianqi
  }
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


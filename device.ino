int relay_pin=8;//继电器
int soil_sign = A0; //土壤湿度信号口
int soil_val = 900; //土壤湿度临界值
int tem_val=26;//温度临界值
#include <dht11.h>
#define DHT11PIN 9
dht11 DHT11;
#include <LiquidCrystal.h>
LiquidCrystal lcd(7,6,5,4,3,2);  //定义脚位
void setup()
{
   Serial.begin(9600); //设置通讯的频率
   pinMode(relay_pin,OUTPUT);//定义继电器输出
    pinMode(soil_sign, INPUT); //定义土壤湿度输入
  lcd.begin(16,2); //设置LCD显示的数目。16 X 2：16格2行。
  lcd.print("Watering System"); //
}
void loop()
{
int chk = DHT11.read(DHT11PIN);
int tem=(int)DHT11.temperature;
int hum=(int)DHT11.humidity;
  int sensorValue = analogRead(soil_sign);   
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
  if((analogRead(soil_sign) >soil_val))
  {
    digitalWrite(relay_pin,HIGH);
delay(3000);
 digitalWrite(relay_pin,LOW); //LOW to poweroff the LED light on jidianqi
  }
 }



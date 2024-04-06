#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "RTClib.h"



RTC_DS1307 RTC;


#define gr_pin 8
#define red_pin 7 

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(gr_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  lcd.begin();
  lcd.backlight();

  RTC.begin(); // load the time from your computer.

if (! RTC.isrunning())

{

Serial.println("RTC is NOT running!");// This will reflect the time that your sketch was compiled

RTC.adjust(DateTime(__DATE__, __TIME__));

}

//Code for esp8266 wifi module.
Serial.println("AT");

Serial.println("");


}
void loop() 
{
  lcd.clear();
  DateTime now = RTC.now();
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1)) //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    digitalWrite(gr_pin, HIGH);
    lcd.print("Checked in");
    cycle();
    esp8266_write();
    
    delay(5000);
    lcd.clear();
    digitalWrite(gr_pin, LOW);
  }
 
 else   {
    Serial.println("Access denied");
    digitalWrite(red_pin, HIGH);
    lcd.print(" Try again");
    cycle();


    delay(5000);
    digitalWrite(red_pin, LOW);
    lcd.clear();
    
      }
  
} 
void cycle(){
  DateTime now = RTC.now();
  lcd.setCursor(0, 1);
  lcd.print(now.month(), DEC);

  lcd.print('/');

  lcd.print(now.day(), DEC);

  lcd.print(' ');

  lcd.print(now.hour(), DEC);
  Serial.print(now.hour(), DEC);
  lcd.print(':');
  Serial.print(":");
  lcd.print(now.minute(), DEC);
  Serial.print(now.minute(), DEC);
  lcd.print(':');
  Serial.print(":");
  lcd.print(now.second(), DEC);
  Serial.print(now.second(), DEC);
  uint16_t year = now.year();
  uint8_t month = now.month();
  uint8_t day = now.day();
  Serial.print(" ");
  switch(weekday(year,month,day)){
      case 1:
      lcd.print("Mon");
      break;
      case 2:
      lcd.print("Tue");
      break;
      case 3:
      lcd.print("Wed");
      break;
      case 4:
      lcd.print("Thu");
      break;
      case 5:
      lcd.print("Fri");
      break;
      case 6:
      lcd.print("Sat");
      break;
      case 7:
      lcd.print("Sun");
      
      break;
      default: lcd.print("Day error!"); break;
      }
      


  }
int weekday(int year,int month,int day)
/* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
{
  int adjustment, mm, yy;
  if (year<2000) year+=2000;
  adjustment = (14 - month) / 12;
  mm = month + 12 * adjustment - 2;
  yy = year - adjustment;
  return (day + (13 * mm - 1) / 5 +
    yy + yy / 4 - yy / 100 + yy / 400) % 7;
}  
void esp8266_write(){
  Serial.println("POST /receiver.php HTTP/1.1");
  Serial.println("Host: 192.168.100.xxx");
  Serial.println("Content-Type: application/x-www-form-urlencoded");
  Serial.println(""); //add rfid code and hour from rtc
  
}
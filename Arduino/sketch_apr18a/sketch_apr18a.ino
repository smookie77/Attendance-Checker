#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <String.h>

#define SSID "POCO X3 Pro" // Configure this according to your WiFi
#define SSID_PASS "paroal12"
#define PHP_IP "192.168.80.230"

#define RST_PIN 9   // See MFRC522 and Arduino pinout
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

RTC_DS3231 rtc;  // Create RTC instance

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Create LCD1602 instance

SoftwareSerial esp(2, 3);
String content;  // Declare content globally

void setup() {
  esp.begin(115200);  // Start the software serial with esp01's default baud rate
  Serial.begin(9600);
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522

  Wire.begin();  // Start the I2C communication
  lcd.begin();   // Init the 1602 LCD display
  lcd.backlight();

  if (!rtc.begin()) {  // Check the RTC module
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  espAdapt();
  espConfig();
  espConnect(10);
  espPHPinit();
}

void espAdapt() {
  esp.println("AT");  // Basic check for "presence" of the esp01
  delay(200);

  esp.println("AT+RST");  // Reset the esp01
  delay(800);

  esp.println("AT+UART_DEF=9600,8,1,0,0");  // Change the esp01's baud rate
  delay(500);

  esp.end();
  esp.begin(9600);  // Adapt the Arduino to the new changes

  esp.println("AT");
  delay(200);

  esp.println("AT+RST");  // Reset the esp01
  delay(800);
}

void espConfig() {
  esp.println("AT+CWMODE=1");  // Setup the esp01 for station mode
  delay(200);

  esp.println("AT+CIPMUX=0");  // Config the esp01 for single connection. Change to 1 for multiple connections
  delay(200);

  esp.println("AT+GMR");  // Displays the esp01's firmware version, make sure you have the Ai-Thinker esp01 firmware flashed
  delay(200);
}

void espConnect(int WifiProtect) {
  esp.println("AT+CWLAP");  // FOR DEBUGGING: LAP = list access points
  delay(6000);
  if (WifiProtect == 1) {
    esp.println("AT+CWJAP=\"" SSID "\",\"" SSID_PASS "\"");  // JAP = join access point, SSID= wifi name, SSID_PASS = wifi password
  } else {
    esp.println("AT+CWJAP=\"" SSID "\"");
  }
  delay(8000);
}

void espPHPinit() {
  esp.println("AT+CIFSR");  // FOR DEBUGGING multiple instances: lists the IP of the esp01 and the access point
  delay(3000);
}

void espSend(String tagUID, int hour, int minute, int seconds) {
  // Construct the GET request
  Serial.println(tagUID);

  String getRequest = "GET /delays.php?chipid=" + tagUID + "&hour=" + String(hour) + "&minute=" + String(minute) + "&seconds=" + String(seconds) + " HTTP/1.1\r\n";
  getRequest += "Host: " + String(PHP_IP) + "\r\n";
  getRequest += "Connection: close\r\n\r\n";

  // Establish TCP connection
  esp.println("AT+CIPSTART=\"TCP\",\"" + String(PHP_IP) + "\",80");
  delay(3000);

  // Send the length of the request
  esp.print("AT+CIPSEND=");
  esp.println(getRequest.length());
  delay(500);

  // Send the request
  esp.print(getRequest);
  delay(1500);

  // Clear content for the next RFID tag
  content = "";

  // Close the connection
  esp.println("AT+CIPCLOSE");
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("SCAN YOUR CHIP");

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    } // Scan the incoming RFID tag
    content.toUpperCase();
    lcd.clear();
    lcd.setCursor(0, 0);
      DateTime now = rtc.now();
      lcd.print("Checked in at:");
      lcd.setCursor(0, 1);
      lcd.print(String(now.hour()));
      lcd.print(":");
      lcd.print(String(now.minute()));
      lcd.print(":");
      lcd.print(String(now.second()));
    String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    espSend(content, now.hour(), now.minute(), now.second()); // Send the tagUID and time to the server
    
  }
  delay(1000); // Wait for stability before scanning another card
}

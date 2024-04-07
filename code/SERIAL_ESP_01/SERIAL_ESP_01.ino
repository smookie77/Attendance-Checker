#include <SoftwareSerial.h>
#define SSID "242.wifi.smooker.org"
#define SSID_PASS "tplinkmotorola12"
#define PHP_IP "192.168.111.198"

SoftwareSerial esp(2,3);
void setup() {
esp.begin(115200);

esp.write("AT+UART_DEF=9600,8,1,0,0\r\n");

esp.end();
delay(200);
esp.begin(9600);

delay(4000);

espInit();
espConnect();
espWebInit();
espSend();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}


void espInit(){


  delay(500);

  esp.write("AT");
  esp.write(13);
  esp.write(10);

  delay(500);

  esp.write("AT+RST");
  esp.write(13);
  esp.write(10);

  delay(3000);

  esp.write("AT");
  esp.write(13);
  esp.write(10);

  delay(100);

  esp.write("AT+CWMODE=1");
  esp.write(13);
  esp.write(10);
  
  delay(500);

  esp.write("AT+CIPMUX=0");
  esp.write(13);
  esp.write(10);

  delay(1000);

  esp.write("AT+GMR\r\n");
  delay(1000);
}

void espConnect(){
  esp.write("AT+CWLAP");
  esp.write(13);
  esp.write(10);

  delay(5000);

  esp.write("AT+CWJAP=\"");
  esp.write(SSID);
  esp.write("\",\"");
  esp.write(SSID_PASS);
  esp.write("\"");
  esp.write(13);
  esp.write(10);

  delay(7000);
  
  
}

void espWebInit(){
    esp.write("AT+CIFSR\r\n");
    delay(400);
    
    esp.write("AT");
    esp.write(13);
    esp.write(10);

    delay(500);

    esp.write("AT+CIPSTART=\"TCP\",\""PHP_IP"\",80");
    esp.write(13);
    esp.write(10);

    delay(2000);


 // esp.write("AT+CIPSERVER=1,80\r\n");
  
}
void espSend(){
  delay(2000);
  esp.write("AT+CIPSEND=108\r\n");
  delay(2000);
  esp.write("GET /reciever/input.php?chip-id=0000000000&hour=12546 HTTP/1.1\r\nHost: 192.168.111.198\r\nConnection: close");
  esp.write(13);
  esp.write(10);
  //esp.write("Host: 192.168.111.22");
//    esp.write(13);
//  esp.write(10);
    esp.write(13);
  esp.write(10);

    delay(1000);

  
}
#include <Wire.h>
#include <SPI.h> 
#include <Adafruit_MCP4725.h>
#include <PS4Controller.h>


#define SDA_1 27    //pins for controlling speed
#define SCL_1 26

#define SDA_2 33    //pins for controlling steer
#define SCL_2 32

int weapon = 22;
int check;

TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

Adafruit_MCP4725 speed;       //initialize mcp4725 responsible for speed
Adafruit_MCP4725 direction;   //initialize mcp4725 responsible for directons/steer

int forward, back, steer;
int forward_value, back_value, steer_value;
int player = 1;
int battery = 0;

void setup()
{
  Serial.begin(115200);
  

  I2Cone.begin(SDA_1, SCL_1, 100000); 
  I2Ctwo.begin(SDA_2, SCL_2, 100000);

  bool status = direction.begin(0x60, &I2Cone);
  bool status1 = speed.begin(0x61, &I2Ctwo);

  pinMode(weapon, OUTPUT);

  Serial.begin(115200);
  PS4.begin();
  Serial.println("Ready.");
}

void loop()
{
  back = 0;
  back =  (PS4.L2Value());
  back_value = (1646-back*6);
  

  forward = 0;
  forward = (PS4.R2Value());
  forward_value = (1646+forward*8);

  if((PS4.L2Value())>0 && (PS4.R2Value())==0)
  speed.setVoltage(back_value, false);

  if((PS4.R2Value())>0 && (PS4.L2Value())==0)
  speed.setVoltage(forward_value, false);
  
  if((PS4.R2Value())==0 && (PS4.L2Value())==0)
  speed.setVoltage(1646, false);

  if((PS4.R2Value())>0 && (PS4.L2Value())>0)
  speed.setVoltage(forward_value, false);

  steer = 0;
  steer = (PS4.LStickX());
  if(steer < -10 || steer >10)
  {
    steer_value = 2048+steer*16;
    direction.setVoltage(steer_value, false);
  }
  else
  {
    direction.setVoltage(2048, false);
  }


  check=0;

  if(PS4.Square())
  digitalWrite(weapon, HIGH);
  else
  digitalWrite(weapon, LOW);
  


  delay(20);

}
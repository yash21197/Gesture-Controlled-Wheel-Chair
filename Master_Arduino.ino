#include<Wire.h>
#include <SoftwareSerial.h>

#define BT_SERIAL_RX 0
#define BT_SERIAL_TX 1
#define horn_led 4
#define horn_pin 12
#define led_pin 7

const int MPU_addr=0x68;
int16_t axis_X,axis_Y,axis_Z;
int minVal=265;
int maxVal=402;

int x;
int y;
int z;

String message="";
int horn=0;
int delayval;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  pinMode(horn_pin,INPUT);
  pinMode(horn_led,OUTPUT);
  pinMode(led_pin,OUTPUT);
  Serial.begin(9600);
}

void loop(){
  x=0;
  y=0;
  z=0;
  horn=0;
  delayval=200;
  
  digitalWrite(led_pin,HIGH);

  if(digitalRead(horn_pin)){
    horn=1;
    digitalWrite(horn_led,HIGH);
  }else{
    horn=0;
    digitalWrite(horn_led,LOW);
  }
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  axis_X=Wire.read()<<8|Wire.read();
  axis_Y=Wire.read()<<8|Wire.read();
  axis_Z=Wire.read()<<8|Wire.read();
    
  int xAng = map(axis_X,minVal,maxVal,-90,90);
  int yAng = map(axis_Y,minVal,maxVal,-90,90);
  int zAng = map(axis_Z,minVal,maxVal,-90,90);
  
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  x = (x>180) ? x-360 : x ;
  y = (y>180) ? y-360 : y ;
  z = (z>180) ? z-360 : z ;

  x = (x>70) ? 70 : x ;
  y = (y>70) ? 70 : y ;
  z = (z>70) ? 70 : z ;
        
  x = (x<-70) ? -70 : x ;
  y = (y<-70) ? -70 : y ;
  z = (z<-70) ? -70 : z ;

  message = "<" + String(x) +","+ String(y) +","+ String(z) + "," + String(horn) + ">";
  Serial.println(message);
  delay(delayval);
}


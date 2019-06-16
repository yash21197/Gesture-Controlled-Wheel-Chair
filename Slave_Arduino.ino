#define led_pin 4

// for ultrasonic sensor
#define trigPin 13
#define echoPin 12

// for piezo buzzer
#define piezoPin 8

// for motor driver chip
#define left_enablePin 11
#define left_in1Pin 10
#define left_in2Pin 9
#define right_enablePin 6
#define right_in1Pin 5
#define right_in2Pin 3

int x=0;
int y=0;
int z=0;
int horn_status=0;
int delayval;

String sx="",sy="",sz="",sh="";
String message="";

int left_speed=0,right_speed=0;
boolean left_reverse=false,right_reverse=false;


void press_Horn(){
    tone(piezoPin, 3000, 100);
    delay(100);
    noTone(piezoPin);
    delayval-=100;
}

void time_To_Buzz(long distance){
  int frequency=3000;   //2000 to 5000
  int duration;
  int delay_val;
  if(distance > 30 || distance < 0){
    Serial.println("Out of range");
    noTone(piezoPin);
  }
  else if(distance <= 30 && distance >= 0){
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm.");
    if(distance >= 0 && distance < 10){
      duration = 120;
      delay_val = 120;
    }
    if(distance >= 10 && distance < 20){
      duration = 80;
      delay_val = 80;
    }
    if(distance >= 20 && distance <= 30){
      duration = 40;
      delay_val = 40;
    }
    tone(piezoPin, frequency, duration);
    delay(delay_val);
    delayval-=delay_val;
    noTone(piezoPin);
    }
}

long distance_Count(){
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

  if(distance >= 200){
    Serial.print("Gretter than range Distane : ");
    distance=200;
    Serial.print(distance);
    Serial.println(" cm");
  }else if(distance<0){
    Serial.print("Smaller than range Distane : ");
    distance=0;
    Serial.print(distance);
    Serial.println(" cm");
  }else{
    Serial.print(distance);
    Serial.println(" cm");
  }
  return distance;
}

void printspeed(){
  Serial.print("L_reverse : ");
  Serial.print(left_reverse);
  Serial.print("  R_reverse : ");
  Serial.print(right_reverse);
  Serial.print("  L_speed : ");
  Serial.print(left_speed);
  Serial.print("  R_speed : ");
  Serial.println(right_speed);
}

void update_Distance_Speed(long distance){
  
  long dis = 30;
  if((distance <= dis && distance >= 0) && (left_reverse == false && right_reverse == false)){
    long dis_diff = distance;
    long persentage = (dis_diff * 100)/dis;
    left_speed = (left_speed * persentage)/100;
    right_speed = (right_speed * persentage)/100;
  }
}

void count_LeftAndRight_Speed(){

  left_reverse = false;
  right_reverse = false;
  left_speed = 0;
  right_speed = 0;

  left_reverse = (x < -20 || (x >= -20 && x <= 20 && y < -20)) ? true : false;
  right_reverse = (x < -20 || (x >= -20 && x <= 20 && y > 20)) ? true : false;
  
  x = (x < 0) ? (-1*x) : x;
  
  left_speed = (x > 20) ? floor((255 * (((x-20)*100)/50) )/ 100) : 0;
  right_speed = (x > 20) ? floor((255 * (((x-20)*100)/50) )/ 100) : 0;
  
  if(x > 20){
    if(y < -20){
      left_speed -= floor((left_speed * ((-1*(y+20)*100)/50) )/ 100);  // degree % out of 50 (70-20)
    }else if(y > 20){
      right_speed -= floor((right_speed * (((y-20)*100)/50) )/ 100);
    }
  }else{
    y = (y < 0) ? (-1*y) : y;
    left_speed = (y > 20) ? floor((255 * (((y-20)*100)/70) )/ 100) : 0;
    right_speed = (y > 20) ? floor((255 * (((y-20)*100)/70) )/ 100) : 0;
  }
  
  // 2/3 the reverse speed always.
  if(left_reverse && right_reverse){
    left_speed = floor((left_speed*2)/3);
    right_speed = floor((right_speed*2)/3);
  }
  
}

void update_Dc_Motor_Speed(){
  analogWrite(left_enablePin, left_speed);
  analogWrite(right_enablePin, right_speed);
  digitalWrite(left_in1Pin, !left_reverse);
  digitalWrite(right_in1Pin, !right_reverse);
  digitalWrite(left_in2Pin, left_reverse);
  digitalWrite(right_in2Pin, right_reverse);
}

boolean check_message(String str){
  int len = str.length();
  if(str.charAt(0=='<' && str.charAt(len-1)=='>')){
    int count=0;
     for(int i=0;i<len;i++){
       if(str.charAt(i)==','){
          count++;
        }
     }
     if(count==3){
       return true;
     }else{
       return false;
     }
  }
}

void getValues(){
  sx="";
  sy="";
  sz="";
  sh="";
  int count=0;
  for(int i=0;i<message.length();i++){
    if(message.charAt(i)!=','){
      if(count==0){
        sx+=message.charAt(i);
      }else if(count==1){
        sy+=message.charAt(i);
      }else if(count==2){
        sz+=message.charAt(i);
      }else if(count==3){
        sh+=message.charAt(i);
      }
    }else
      count++;
  }
}

void update_xyzh(){
  x=0;
  y=0;
  z=0;
  horn_status=0;

  while(Serial.available()){
    char c = char(Serial.read());
    message+=c;
  }
  if(!Serial.available()){
    if(message != ""){
      Serial.println(message);
      if(check_message(message)){
        message.replace("<","");
        message.replace(">","");

        getValues();

        x = sx.toInt();
        y = sy.toInt();
        z = sz.toInt();
        horn_status = sh.toInt();

        Serial.println(x);
        Serial.println(y);
        Serial.println(z);
        Serial.println(horn_status);
      }
    }else{
      Serial.println("no message...");
    }
    message="";
  }
}


// setup pins

void setup_Ultra_Sonic_Sensor(){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void setup_Piezo_Buzzer(){
  pinMode(piezoPin, OUTPUT);
}

void setup_Dc_Motor_Driver(){
  pinMode(left_in1Pin, OUTPUT);
  pinMode(left_in2Pin, OUTPUT);
  pinMode(left_enablePin, OUTPUT);
  pinMode(right_in1Pin, OUTPUT);
  pinMode(right_in2Pin, OUTPUT);
  pinMode(right_enablePin, OUTPUT);
}

void setup() {
  Serial.begin(9600);
  pinMode(led_pin,OUTPUT);
  setup_Ultra_Sonic_Sensor();
  setup_Piezo_Buzzer();
  setup_Dc_Motor_Driver();
}

void loop() {
  delayval=200;
  digitalWrite(led_pin,HIGH);
  long distance = distance_Count();
  update_xyzh();
  count_LeftAndRight_Speed();
  update_Distance_Speed(distance);
  printspeed();
  update_Dc_Motor_Speed();
  if(horn_status)
    press_Horn();
  if(!horn_status)
    time_To_Buzz(distance);
   delay(delayval);
}


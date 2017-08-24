#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const int SpeedPin = 9;
const int FloSensor = A0;
const int VegSensor = A1;

//Key connections with arduino
const int up_key=10;
const int down_key=11;
const int less_key=12;
const int plus_key=8;

int setTempVal = 26;
int FloTemp = 0;
int VegTemp = 0;
int IncPerc = 5; //Increment by X%
int MaxSpeedPerc = 25;
int MinSpeedPerc = 05;
int SpeedVal = 0;
int HysteresisPerc = 0;

// create the menu counter
int menuCount   = 1;
int menuCountMax = 4;

void getData() {
  int FloReading = analogRead(FloSensor);  
  float FloVoltage = FloReading * 5.0;
  FloVoltage /= 1024.0; 
  FloTemp =(int) round((FloVoltage - 0.5) * 100);
  int VegReading = analogRead(VegSensor);  
  float VegVoltage = VegReading * 5.0;
  VegVoltage /= 1024.0; 
  VegTemp =(int) round((VegVoltage - 0.5) * 100); 
  
}

void setTemp() {
  if (digitalRead(less_key) == LOW)
  {
    if (setTempVal > 0)
    {
      --setTempVal;    
    }
    lcd.clear();
  }
  
  if (digitalRead(plus_key) == LOW)
  {
    if(setTempVal < 30)
    {
      ++setTempVal;
    }
    lcd.clear();
  }
}  

void setMaxSpeed() {
  if (digitalRead(less_key) == LOW)
  {
    if (MaxSpeedPerc > 0)
    {
      MaxSpeedPerc -= IncPerc;    
    }
    lcd.clear();
  }
  
  if (digitalRead(plus_key) == LOW)
  {
    if(MaxSpeedPerc < 100)
    {
      MaxSpeedPerc += IncPerc;
    }
    lcd.clear();
  }
}

void setMinSpeed() {
  if (digitalRead(less_key) == LOW)
  {
    if (MinSpeedPerc > 0)
    {
      MinSpeedPerc -= IncPerc;    
    }
    lcd.clear();
  }
  
  if (digitalRead(plus_key) == LOW)
  {
    if(MinSpeedPerc < 100)
    {
      MinSpeedPerc += IncPerc;
    }
    lcd.clear();
  }
}

void printHome() {
  lcd.setCursor(0, 0);
  lcd.print("Flo: ");
  lcd.print(FloTemp);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Veg: ");
  lcd.print(VegTemp);
  lcd.print("C");
}

void printTemp() {
  lcd.setCursor(0, 0);
  lcd.print("Set Temp.");
  lcd.setCursor(0,  1);
  lcd.print(setTempVal);
  lcd.print("C");
}

void printMaxSpeed() {
  lcd.home();
  lcd.print("Set Max Speed");
  lcd.setCursor(0, 1);
  lcd.print(MaxSpeedPerc);
  lcd.print("%");
}

void printMinSpeed() {
  lcd.home();
  lcd.print("Set Min Speed");
  lcd.setCursor(0, 1);
  lcd.print(MinSpeedPerc);
  lcd.print("%");
}

//=================================================================
//                  SETUP
//=================================================================
void setup() {
  pinMode(SpeedPin, OUTPUT);
  pinMode(up_key,INPUT);
  pinMode(down_key,INPUT);
  pinMode(less_key,INPUT);
  pinMode(plus_key,INPUT);
  
  digitalWrite(up_key,HIGH); //Pull up 
  digitalWrite(down_key,HIGH);
  digitalWrite(less_key,HIGH);
  digitalWrite(plus_key,HIGH);//Pull up
  
  lcd.begin(16, 2);
  lcd.print(" ClimateControl");
  delay(1500);
  lcd.clear();
}

//=================================================================
//                  LOOP
//================================================================= 
void loop() {
  
  if(digitalRead(down_key)==LOW)
  {
    if(menuCount < menuCountMax)
    {
      menuCount++;    
    }
    else {
      menuCount = menuCountMax;
    }
    lcd.clear();
  }
  
  if(digitalRead(up_key)==LOW)
  {
    if(menuCount > 1)
    {
      menuCount--;
    }
    else {
      menuCount = 1;
    }
    lcd.clear();
  }
  
  if(menuCount == 1) {
    getData();
    printHome();
  }
  
  if(menuCount == 2) {
    setTemp();
    printTemp();
  }
  
  if(menuCount == 3) {
    setMaxSpeed();
    printMaxSpeed();
  }
  
  if(menuCount == 4) {
    setMinSpeed();
    printMinSpeed();
  }
  
  HysteresisPerc = (MaxSpeedPerc-MinSpeedPerc)/5;
  if (FloTemp <= setTempVal) {
    SpeedVal = map(MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (FloTemp > setTempVal && FloTemp <= setTempVal+1) {
    SpeedVal = map(HysteresisPerc+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (FloTemp > setTempVal+1 && FloTemp <= setTempVal+2) {
    SpeedVal = map((HysteresisPerc*2)+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (FloTemp > setTempVal+2 && FloTemp <= setTempVal+3) {
    SpeedVal = map((HysteresisPerc*3)+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (FloTemp > setTempVal+3 && FloTemp <= setTempVal+4) {
    SpeedVal = map((HysteresisPerc*4)+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (FloTemp > setTempVal+4) {
    SpeedVal = map(MaxSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
}

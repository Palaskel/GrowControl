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

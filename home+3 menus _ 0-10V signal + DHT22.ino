#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPINflo 14     // what digital pin we're connected to
#define DHTPINveg 15

#define DHTTYPEflo DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPEveg DHT22

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dhtflo(DHTPINflo, DHTTYPEflo);
DHT dhtveg(DHTPINveg, DHTTYPEveg);

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

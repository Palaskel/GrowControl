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

void Sensor() {
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  hflo =(float) dhtflo.readHumidity();
  hveg =(float) dhtveg.readHumidity();
  // Read temperature as Celsius (the default)
  tflo =(float) dhtflo.readTemperature();
  tveg =(float) dhtveg.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(hflo) || isnan(tflo)) {
    Serial.println("Failed to read from DHTflo sensor!");
    return;
  }
  if (isnan(hveg) || isnan(tveg)) {
    Serial.println("Failed to read from DHTveg sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.print(hflo);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(tflo);
  Serial.print(" *C ");
  Serial.print("Humidity: ");
  Serial.print(hveg);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(tveg);
  Serial.print(" *C ");
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
  dhtflo.begin();
  dhtveg.begin
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
    Sensor(); //DHT22 void
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
  if (tflo <= setTempVal) {
    SpeedVal = map(MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (tflo > setTempVal && tflo <= setTempVal+1) {
    SpeedVal = map(HysteresisPerc+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (tflo > setTempVal+1 && tflo <= setTempVal+2) {
    SpeedVal = map((HysteresisPerc*2)+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (tflo > setTempVal+2 && tflo <= setTempVal+3) {
    SpeedVal = map((HysteresisPerc*3)+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (tflo > setTempVal+3 && tflo <= setTempVal+4) {
    SpeedVal = map((HysteresisPerc*4)+MinSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
  else if (tflo > setTempVal+4) {
    SpeedVal = map(MaxSpeedPerc, 0, 100, 0, 255);
    analogWrite(SpeedPin, SpeedVal);
  }
}


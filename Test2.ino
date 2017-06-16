//sketch adjusted based on GrowGreen sketch by AvidLerner
// GrowGreen/GrowGreen.ino 

#include <Wire.h>
#include <EEPROMVar.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <dht11.h>  
// setup DHT for use
//dht11 DHT;
// DHT data  pin
#define DHT11_PIN 0 // ADC0 for Uno
int DHT11Temp = 0;
int DHT11Hum = 0;
/* completed 6/23/16 executes the menu system, display date, time, 
temperature and humidity and runs throuhg screens and loops correctly.
define some values used by the panel and buttons
 readButton for each function */
 // The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
// set up backlight
int bklDelay    = 100000;    // ms for the backlight to idle before turning off
unsigned long bklTime = 0;  // counter since backlight turned on
// create the menu counter
//int menuCount   = 1;
//int menuSelect = 0;

//create the plus and minus navigation delay counter with its initial maximum of 250.
//byte btnMaxDelay = 200;
//byte btnMinDelay = 25;
//byte btnMaxIteration = 5;
//byte btnCurrIteration;
// setup[ ds1307  Real Time Clock using i2c address direct
#define DS1307_I2C_ADDRESS 0x68
// backlight on/off
#define ON 0x1
#define OFF 0x0
// lcd screen variable
int LCD_R=16;  // lcd rows
int LCD_C=2;  // lcd columns
//create manual override variables
//boolean override = false;
//byte overmenu = 0;
//int overpercent = 0;
// button read variables for lcd shield
//uint8_t i=0;
//uint8_t buttons =0;
//------------------- system variables-----------------------//
int minCounter = 0;         // counter that resets at midnight.
int oldMinCounter = 0;      // counter that resets at midnight.
int oneLed = 9;             // pin for channel 1
int twoLed = 10;            // pin for channel 2
int threeLed = 11;          // pin for channel 3
int fourLed = 5;            // pin for channel 4
//int fiveLed = 6;            // pin for channel 5
//int sixLed = 7;             // pin for channel 6
int oneVal = 0;             // current value for channel 1
int twoVal = 0;             // current value for channel 2
int threeVal = 0;           // current value for channel 3
int fourVal = 0;            // current value for channel 4
//int fiveVal = 0;            // current value for channel 5
//int sixVal = 0;             // current value for channel 6
// dht11 data variables for printing temp in farhenheit or celcius
//int dhtFTemp=0;
int dhtCTemp=0;
int dhtHumidity=0;
double dhtDTemp=0;
double dhtDHumidity=0;
// Variables making use of EEPROM memory:
EEPROMVar<int> oneStartMins = 360;      // minute to start this channel.
EEPROMVar<int> onePhotoPeriod = 720;   // photoperiod in minutes for this channel.
EEPROMVar<int> oneMax = 100;           // max intensity for this channel, as a percentage
EEPROMVar<int> oneFadeDuration = 60;   // duration of the fade on and off for sunrise and sunset for
                                       //    this channel.
EEPROMVar<int> twoStartMins = 360;  //  6:am
EEPROMVar<int> twoPhotoPeriod = 720; //
EEPROMVar<int> twoMax = 100;
EEPROMVar<int> twoFadeDuration = 60;

EEPROMVar<int> threeStartMins = 360;
EEPROMVar<int> threePhotoPeriod = 720;
EEPROMVar<int> threeMax = 100;
EEPROMVar<int> threeFadeDuration = 60;
                            
EEPROMVar<int> fourStartMins = 1050;  // 6:pm
EEPROMVar<int> fourPhotoPeriod = 720;  
EEPROMVar<int> fourMax = 100;          
EEPROMVar<int> fourFadeDuration = 60; 

//EEPROMVar<int> fiveStartMins = 1050;
//EEPROMVar<int> fivePhotoPeriod = 720;  
//EEPROMVar<int> fiveMax = 100;          
//EEPROMVar<int> fiveFadeDuration = 60; 

//EEPROMVar<int> sixStartMins = 1050;
//EEPROMVar<int> sixPhotoPeriod = 720;  
//EEPROMVar<int> sixMax = 100;          
//EEPROMVar<int> sixFadeDuration = 60; 
// variables to invert the output PWM signal,
// for use with drivers that consider 0 to be "on"
// i.e. buckpucks. If you need to provide an inverted 
// signal on any channel, set the appropriate variable to true.
boolean oneInverted = false;
boolean twoInverted = false;
boolean threeInverted = false;
boolean fourInverted = false; 
boolean fiveInverted = false;
boolean sixInverted = false; 

int state = 0;       // state of keypress master state = 1 means read keypres
int h = 0;  // hours
int m = 0;  // minutes
int s = 0;  // seconds
//-----------print functions ------------------/
void PrintBegin()
{
  lcd.begin(LCD_R, LCD_C);   // start the library  
  lcd.setBacklight(ON);
   
}
//void printTempMenu()
//  {
//  lcd.setCursor(12,0);
//  lcd.print(dhtCTemp);
//  lcd.setCursor(15,0);
//  lcd.print((char)223);
//  //lcd.setCursor(13,0);
//  //lcd.print(dhtHumidity);
//  //lcd.setCursor(15,0);
//  //lcd.print("%");
//  }
  void printTempHumidityMenu()
  {
  lcd.setCursor(9,0);
  lcd.print(dhtCTemp);
  lcd.setCursor(12,0);
  lcd.print((char)223);
  lcd.setCursor(13,0);
  lcd.print(dhtHumidity);
  lcd.setCursor(15,0);
  lcd.print("%");
  }
//  setup serial printer for use
void SPrintBegin()
  {
  Serial.begin(9600);
  Serial.println("Ready"); 
  }

// clean lcd screen
void cleanScreen()
{
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);
  lcd.print("                ");
}
// readButtons from lcd shield
//void ReadButtons()
//   {
//    uint8_t buttons = lcd.readButtons();
//
//  if (buttons) {
//    //lcd.clear();
//    //lcd.setCursor(0,0);
//    if (buttons & BUTTON_UP) {
//     
//    }
//    if (buttons & BUTTON_DOWN) {
//    
//    }
//    if (buttons & BUTTON_LEFT) {
//     
//    }
//    if (buttons & BUTTON_RIGHT) {
//     
//    }
//    if (buttons & BUTTON_SELECT) {
//      
//    }
//  }
//}
// end ReadButtons
/****** RTC Functions ******/
/***************************/

// Convert decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}
// Sets date and time, starts the clock
void setDate(byte second,        // 0-59
             byte minute,        // 0-59
             byte hour,          // 1-23
             byte dayOfWeek,     // 1-7
             byte dayOfMonth,    // 1-31
             byte month,         // 1-12
             byte year)          // 0-99
{
   Wire.beginTransmission(DS1307_I2C_ADDRESS);
   Wire.write(0);
   Wire.write(decToBcd(second));
   Wire.write(decToBcd(minute));
   Wire.write(decToBcd(hour));
   Wire.write(decToBcd(dayOfWeek));
   Wire.write(decToBcd(dayOfMonth));
   Wire.write(decToBcd(month));
   Wire.write(decToBcd(year));
   Wire.endTransmission();
}

// Gets the date and time
void getDate(byte *second,
             byte *minute,
             byte *hour,
             byte *dayOfWeek,
             byte *dayOfMonth,
             byte *month,
             byte *year)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}
/****** LED Functions ******/
/***************************/
//function to set LED brightness according to time of day
//function has three equal phases - ramp up, hold, and ramp down

int   setLed(int mins,         // current time in minutes
            int ledPin,        // pin for this channel of LEDs
            int start,         // start time for this channel of LEDs
            int period,        // photoperiod for this channel of LEDs
            int fade,          // fade duration for this channel of LEDs
            int ledMax,        // max value for this channel
            boolean inverted   // true if the channel is inverted
            )  {
  int val = 0;
      
      //fade up
      if (mins > start || mins <= start + fade)  {
        val = map(mins - start, 0, fade, 0, ledMax);
      }
      //fade down
      if (mins > start + period - fade && mins <= start + period)  {
        val = map(mins - (start + period - fade), 0, fade, ledMax, 0);
      }
      //off or post-midnight run.
      if (mins <= start || mins > start + period)  {
        if((start+period)%1440 < start && (start + period)%1440 > mins )
          {
            val=map((start+period-mins)%1440,0,fade,0,ledMax);
          }
        else  
        val = 0; 
      }
    
    
    if (val > ledMax)  {val = ledMax;} 
    if (val < 0) {val = 0; } 
    
  if (inverted) {analogWrite(ledPin, map(val, 0, 100, 255, 0));}
  else {analogWrite(ledPin, map(val, 0, 100, 0, 255));}
  if(override){val=overpercent;}
  return val;
}
/**** Display Functions ****/
/***************************/

//button hold function
//int btnCurrDelay(byte curr)
//{
//  if(curr==btnMaxIteration)
//  {
//    btnCurrIteration = btnMaxIteration;
//    return btnMaxDelay;
//  }
//  else if(btnCurrIteration ==0)
//  {
//    return btnMinDelay;
//  }
//  else
//  {
//    btnCurrIteration--;
//    return btnMaxDelay;
//  }
//}

// format a number of minutes into a readable time (24 hr format)
void printMins(int mins,       //time in minutes to print
               boolean ampm    //print am/pm?
              )  {
  int hr = (mins%1440)/60;
  int mn = mins%60;
    if(hr<10){
      lcd.print(" ");
    }
    lcd.print(hr);
    lcd.print(":");
    if(mn<10){
      lcd.print("0");
    }
    lcd.print(mn); 
}

// format hours, mins, secs into a readable time (24 hr format)
void printHMS (byte hr,
               byte mn,
               byte sec      //time to print
              )  
{
  
    if(hr<10){
      lcd.print(" ");
    }
    lcd.print(hr, DEC);
    lcd.print(":");
    if(mn<10){
      lcd.print("0");
    }
    lcd.print(mn, DEC);
    //lcd.print(":");
    //if(sec<10){
      //lcd.print("0");
    //}
    //lcd.print(sec, DEC);
}
void ovrSetAll(int pct){
    analogWrite(oneLed,map(pct,0,100,0,255));
    analogWrite(twoLed,map(pct,0,100,0,255));
    analogWrite(threeLed,map(pct,0,100,0,255));
    analogWrite(fourLed,map(pct,0,100,0,255));
//   analogWrite(fiveLed,map(pct,0,100,0,255));
//   analogWrite(sixLed,map(pct,0,100,0,255));
}
// setup ddrc and portc for temp/humidity sensor
// DHT11_GetData Use with Mega 2560 simple and direct
/*
void GetDHT11Data()
{
int chk = DHT.read(DHT11_PIN);    // READ DATA
  DHT11Temp = (int) round(DHT.temperature);
  dhtHumidity = DHT.humidity;
  dhtFTemp =  (int)round((1.8*DHT11Temp)+32);
}
// use for Uno compile does not work with Mega 2560
*/
void DHTSetup(){
   DDRC |= _BV(DHT11_PIN);
  PORTC |= _BV(DHT11_PIN);
  }
// read dht11 data line for input get current temperature and humidity
byte read_dht11_dat()
{
  
  byte i = 0;
  byte result=0;
  for(i=0; i< 8; i++)
  {
    while(!(PINC & _BV(DHT11_PIN)));  // wait for 50us
    delayMicroseconds(30);
    if(PINC & _BV(DHT11_PIN)) 
      result |=(1<<(7-i));
    while((PINC & _BV(DHT11_PIN)));  // wait '1' finish
    }
    return result;
}

// read data for input from dht11 temp/humidity sensor 
// use for Uno compile does not work with mega 2560

void GetDHTData()
  {
  byte dht11_dat[5];
  byte dht11_in;
  byte i;// start condition
   // 1. pull-down i/o pin from 18ms
  PORTC &= ~_BV(DHT11_PIN);
  delay(18);
  PORTC |= _BV(DHT11_PIN);
  delayMicroseconds(40);
  DDRC &= ~_BV(DHT11_PIN);
  delayMicroseconds(40);
  
  dht11_in = PINC & _BV(DHT11_PIN);
  if(dht11_in)
  {
   // Serial.println("dht11 start condition 1 not met");
    
    return;
  }
  delayMicroseconds(80);
  dht11_in = PINC & _BV(DHT11_PIN);
  if(!dht11_in)
  {
    //Serial.println("dht11 start condition 2 not met");
   
    return;
  }
  
  delayMicroseconds(80);// now ready for data reception
  for (i=0; i<5; i++)
    dht11_dat[i] = read_dht11_dat();
  DDRC |= _BV(DHT11_PIN);
  PORTC |= _BV(DHT11_PIN);
  byte dht11_check_sum = dht11_dat[0]+dht11_dat[1]+dht11_dat[2]+dht11_dat[3];// check check_sum
  if(dht11_dat[4]!= dht11_check_sum)
  
  {
    //Serial.println("DHT11 checksum error");
    
  }
  dhtCTemp=(int) round(dht11_dat[2]);
  //double dhtDTemp= double dht11_dat[2];
  dhtFTemp=(int)round(1.8*dht11_dat[2]+32);
  dhtHumidity=(int) dht11_dat[0];
  //double dhtDHumidity= double dht11_dat[0];
  } // end read data DHT11

// showChannelValues()  print channel values to lcd
//void showChannelValues() { // display the current intensity for each led channel
//byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
//char buffer[17];
//lcd.noCursor();
//lcd.noBlink();
//getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
//lcd.noCursor();
//lcd.noBlink();
//lcd.setCursor(0,0);
//sprintf(buffer, "%3d %3d %3d", oneVal, twoVal, threeVal);
//lcd.print(buffer);
//lcd.setCursor(0,1);
//sprintf(buffer, "%3d %3d %3d%2d:%02d", fourVal, fiveVal, sixVal, hour, minute);
//lcd.print(buffer);
//}
// end showChannelValues

// void setup
void setup()
{
Wire.begin();
//DHTSetup();

// setup lcd screen object
PrintBegin();
// set serial printer
SPrintBegin();
//splash screen
lcd.print("GrowControl");
delay(5000);
lcd.clear();
//lcd.setBacklight(OFF);
//analogWrite(bkl,bklIdle);
btnCurrIteration = btnMaxIteration;


} // end setup

// Main loop
void loop(){ 
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  getDate(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  oldMinCounter = minCounter;
  minCounter = hour * 60 + minute;
  
  //check & set fade durations
  if(oneFadeDuration > onePhotoPeriod/2 && onePhotoPeriod >0){oneFadeDuration = onePhotoPeriod/2;}
  if(oneFadeDuration<1){oneFadeDuration=1;}
  
  if(twoFadeDuration > twoPhotoPeriod/2 && twoPhotoPeriod >0){twoFadeDuration = twoPhotoPeriod/2;} 
  if(twoFadeDuration<1){twoFadeDuration=1;}
  
  if(threeFadeDuration > threePhotoPeriod/2 && threePhotoPeriod >0){threeFadeDuration = threePhotoPeriod/2;}
  if(threeFadeDuration<1){threeFadeDuration=1;}
  
  if(fourFadeDuration > fourPhotoPeriod/2 && fourPhotoPeriod > 0){fourFadeDuration = fourPhotoPeriod/2;}
  if(fourFadeDuration<1){fourFadeDuration=1;}
 
  //if(fiveFadeDuration > fivePhotoPeriod/2 && fivePhotoPeriod >0){fiveFadeDuration = fivePhotoPeriod/2;}
  //if(fiveFadeDuration<1){fiveFadeDuration=1;}
  
  //if(sixFadeDuration > sixPhotoPeriod/2 && sixPhotoPeriod > 0){sixFadeDuration = sixPhotoPeriod/2;}
  //if(sixFadeDuration<1){sixFadeDuration=1;} 
  
  //check & set any time functions
  
  
  //set outputs
  if(!override){
  oneVal = setLed(minCounter, oneLed, oneStartMins, onePhotoPeriod, oneFadeDuration, oneMax, oneInverted);
  twoVal = setLed(minCounter, twoLed, twoStartMins, twoPhotoPeriod, twoFadeDuration, twoMax, twoInverted);
  threeVal = setLed(minCounter, threeLed, threeStartMins, threePhotoPeriod, threeFadeDuration, threeMax, threeInverted);
  fourVal = setLed(minCounter, fourLed, fourStartMins, fourPhotoPeriod, fourFadeDuration, fourMax, fourInverted);
  //fiveVal = setLed(minCounter, fiveLed, fiveStartMins, fivePhotoPeriod, fiveFadeDuration, fiveMax, fiveInverted);
  //sixVal = setLed(minCounter, sixLed, sixStartMins, sixPhotoPeriod, sixFadeDuration, sixMax, sixInverted);
  
  }
  else{
    ovrSetAll(overpercent);
  }
  
}  // end loop
// end of sketch


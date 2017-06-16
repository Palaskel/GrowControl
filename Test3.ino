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

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// setup[ ds1307  Real Time Clock using i2c address direct
#define DS1307_I2C_ADDRESS 0x68

// backlight on/off
#define ON 0x1
#define OFF 0x0

// lcd screen variable
int LCD_R=16;  // lcd rows
int LCD_C=2;  // lcd columns

//------------------- system variables-----------------------//
int minCounter = 0;         // counter that resets at midnight.
int oldMinCounter = 0;      // counter that resets at midnight.
int oneDimm = 9;             // pin for channel 1
int twoDimm = 10;            // pin for channel 2
int oneDimmVal = 0;             // current value for channel 1
int twoDimmVal = 0;             // current value for channel 2
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
EEPROMVar<int> oneFadeDuration = 60;   // duration of the fade on and off for sunrise and sunset for this channel

EEPROMVar<int> twoStartMins = 360;  //  6:am
EEPROMVar<int> twoPhotoPeriod = 720; //
EEPROMVar<int> twoMax = 100;
EEPROMVar<int> twoFadeDuration = 60;

// variables to invert the output PWM signal,
// for use with drivers that consider 0 to be "on"
// i.e. buckpucks. If you need to provide an inverted 
// signal on any channel, set the appropriate variable to true.
boolean oneInverted = false;
boolean twoInverted = false;

int h = 0;  // hours
int m = 0;  // minutes
int s = 0;  // seconds

//-----------print functions ------------------/
void PrintBegin()
{
  lcd.begin(LCD_R, LCD_C);   // start the library  
  lcd.setBacklight(ON);
   
}
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

void SPrintBegin() //  setup serial printer for use
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

void DHTSetup(){
   DDRC |= _BV(DHT11_PIN);
  PORTC |= _BV(DHT11_PIN);
  }

byte read_dht11_dat() // read dht11 data line for input get current temperature and humidity
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

void GetDHTData() // read data for input from dht11 temp/humidity sensor 
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

/****** LED Functions ******/
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
  return val;
}

// void setup
void setup()
{
Wire.begin();
//DHTSetup();
PrintBegin(); //setup lcd screen object
SPrintBegin(); //set serial printer
lcd.print("GrowControl"); //splash screen
delay(5000);
lcd.clear();
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
 
  //check & set any time functions
   
  //set outputs
  if(!override){
  oneVal = setLed(minCounter, oneLed, oneStartMins, onePhotoPeriod, oneFadeDuration, oneMax, oneInverted);
  twoVal = setLed(minCounter, twoLed, twoStartMins, twoPhotoPeriod, twoFadeDuration, twoMax, twoInverted);
  threeVal = setLed(minCounter, threeLed, threeStartMins, threePhotoPeriod, threeFadeDuration, threeMax, threeInverted);
  fourVal = setLed(minCounter, fourLed, fourStartMins, fourPhotoPeriod, fourFadeDuration, fourMax, fourInverted);
  
  }
  else{
    ovrSetAll(overpercent);
  }
  
}  // end loop
// end of sketch


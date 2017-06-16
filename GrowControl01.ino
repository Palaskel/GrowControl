// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
// 2010-02-04 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 RTC;

void setup () {
    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
}

void loop () {
    DateTime now = RTC.now();  //getting time from RTC
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}    

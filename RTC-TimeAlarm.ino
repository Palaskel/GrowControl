#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

int oneRelay = 6;
int twoRelay = 7;

void setup()  {
  Serial.begin(9600);
  while (!Serial) ;           // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");      
  pinMode(oneRelay, OUTPUT);
  pinMode(twoRelay, OUTPUT);
  // create the alarms, to trigger at specific times
  Alarm.alarmRepeat(9,00,0, oneRelayON);  // 8:30am every day
  Alarm.alarmRepeat(21,00,0, oneRelayOFF); // 5:45pm every day
  Alarm.alarmRepeat(6,00,0, twoRelayON);
  Alarm.alarmRepeat(0,00,0, twoRelayOFF);
}

void loop()
{
  if (timeStatus() == timeSet) {
    digitalClockDisplay();
  } else {
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
    delay(4000);
  }
  delay(1000);
}

// functions to be called when an alarm triggers:
void oneRelayON() {
  Serial.println("Alarm: - turn lights on");
  digitalWrite(oneRelay, HIGH);
}

void oneRelayOFF() {
  Serial.println("Alarm: - turn lights off);
  digitalWrite(oneRelay, LOW);
}

void twoRelayON() {
  Serial.println("Alarm: - turn lights on");
  digitalWrite(twoRelay, HIGH);
}
                 
void twoRelayOFF() {
  Serial.println("Alarm: - turn lights off");
  digitalWrite(twoRelay, LOW);
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

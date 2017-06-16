#include <dht11.h>

dht11 DHT11;

#define oneDHT11 0
#define twoDHT11 1

void setup()
{
  Serial.begin(9600);
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
}

void loop()
{
  Serial.println("\n");

  int onechk = DHT11.read(oneDHT11);
  int twochk = DHT11.read(twoDHT11);

  Serial.print("Read sensor one: ");
  switch (onechk)
  {
    case DHTLIB_OK: 
		Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
    default: 
		Serial.println("Unknown error"); 
		break;
  }

  Serial.print("ONE Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("ONE Temperature (°C): ");
  Serial.println((float)DHT11.temperature, 2);
  
  Serial.print("Read sensor two: ");
  switch (twochk)
  {
    case DHTLIB_OK: 
		Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
    default: 
		Serial.println("Unknown error"); 
		break;
  }

  Serial.print("TWO Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("TWO Temperature (°C): ");
  Serial.println((float)DHT11.temperature, 2);

  delay(2000);
}

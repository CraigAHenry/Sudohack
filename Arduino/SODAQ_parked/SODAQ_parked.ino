#include <Sodaq_RN2483.h>
#include "Arduino.h"

#include <Adafruit_GPS.h>



#define debugSerial SerialUSB
#define loraSerial Serial2

Adafruit_GPS GPS(&Serial);

#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
//#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
//boolean usingInterrupt = false;

//void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

const uint8_t devAddr[4] =
{
0x3F,0x16,0xEF,0xFC
};

// Set the Network session key to your value
const uint8_t nwkSKey[16] =
{
0x1F,0xDE,0x0D,0x08,0xDC,0x76,0x3B,0xA4,0x2E,0x7A,0x52,0xE1,0x6D,0x18,0x9E,0x1E
};

//Set the Application session key to your value
const uint8_t appSKey[16] =
{
0xC5,0x59,0x08,0xB4,0x5F,0xBB,0xC1,0xCD,0x15,0xE0,0x4C,0x53,0x89,0xE3,0x19,0xC3
};

// defines pins numbers
const int trigPin = 5;
const int echoPin = 6;
// defines variables
long duration;
int distance;

//Set up some values and labels to send
int counter = 0 ;
int wait = 7000;
int initialTime = 0;
int deltaTime = 0;

char payloadToSend[12];
uint8_t buff[4];

void setup()
{
  fixHardwareIssue();

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  setLEDPins();

  startUpLEDFlash();

  connectSerials();
  connectLORA();

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
} 

void ultraSound()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
}

uint32_t floatToInt(float fl)
{
  uint32_t *tempInt = (uint32_t *)&fl;

  return *tempInt;
}


void thirtyTwoToEight(const uint32_t &buffer, uint8_t* eightBit)
{
    uint8_t bitMaskEight = 255;
    uint32_t bitMask;

    for (int i = 0; i < 4; i++)
    {
        bitMask = bitMaskEight;
        bitMask = bitMask << (i * 8);
        eightBit[3-i] = (uint8_t)((bitMask & buffer) >> i * 8);
    }
}
void loop()
{
  //get the temperature
  //float tempValue = getTemperature();
//  ultraSound();
//  debugSerial.print("Distance: ");
//  debugSerial.println(distance);
  GPS.read();

  if(GPS.newNMEAreceived())
  {
    GPS.parse(GPS.lastNMEA());
  }
  
  if (millis() >= initialTime + wait)
  {
    initialTime = millis();
//    uint32_t GPSTime = 60*60*GPS.hour + 60*GPS.minute + GPS.seconds;
//    uint32_t lat = GPS.latitude_fixed;
//    uint32_t lon = GPS.longitude_fixed;

    uint32_t GPSTime = 12;
    uint32_t lat = 34;
    uint32_t lon = 56;


    thirtyTwoToEight(GPSTime, buff);
    for(int i = 0; i < 4; i++)
    {
      payloadToSend[i] = buff[i];
      debugSerial.println((uint8_t)payloadToSend[i]);
    }
    thirtyTwoToEight(lat, buff);
    for(int i = 0; i < 4; i++)
    {
      payloadToSend[i + 4] = buff[i];
      debugSerial.println((uint8_t)payloadToSend[i+4]);
    }    
    thirtyTwoToEight(lon, buff);
    for(int i = 0; i < 4; i++)
    {
      payloadToSend[i + 8] = buff[i];
      debugSerial.println((uint8_t)payloadToSend[i+8]);
    }
    
    // Send the payload
    loraSend(*payloadToSend, wait);
    counter++;
  }


  
}


#include <Sodaq_RN2483.h>

#include "Arduino.h"

#include <Adafruit_GPS.h>
#include "RunningMedian.h"
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
int wait = 8000;
int initialTime = 0;
int deltaTime = 0;
const int sizeMessage = 42;
RunningMedian distances = RunningMedian(100);

struct __attribute__((packed,aligned(1))) info 
{
  uint32_t GPSTime[3];
  uint32_t lat[3];
  uint32_t lon[3];
  uint16_t distance[3];
} payload;

int distanceSum, distanceAvg;
int measureWait = 2500;
int measureCount = 0;
int sampleWait = measureWait / 100;
int sampleCount = 0;


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


void loop()
{
  GPS.read();

  if(GPS.newNMEAreceived())
  {
    GPS.parse(GPS.lastNMEA());
  }
  if(millis() >= initialTime + sampleWait * (sampleCount + 1) && sampleCount <= 100)
  {
    int val = ultraSound();
    if( val <= 400)
    {
      distances.add(val);
    }
    else
    {
      distances.add(401);
    }
    sampleCount++;
  }

  
  if(millis() >= initialTime + measureWait * (measureCount + 1))
  {
    payload.GPSTime[measureCount] = 60*60*GPS.hour + 60*GPS.minute + GPS.seconds;
    payload.lat[measureCount] = GPS.latitude_fixed;
    payload.lon[measureCount] = GPS.longitude_fixed;
    payload.distance[measureCount] = distances.getMedian();

    debugSerial.println(payload.lat[measureCount]);
    debugSerial.println(payload.lon[measureCount]);
    measureCount++;
    sampleCount = 0;
  }


  if (millis() >= initialTime + wait)
  {
    initialTime = millis();

     //Send the payload
    loraSend((uint8_t*) &payload, wait, sizeMessage);
    measureCount = 0;
  } 
}


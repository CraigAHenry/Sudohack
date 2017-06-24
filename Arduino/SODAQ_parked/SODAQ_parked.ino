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

//Set up some values and labels to send
int counter = 0 ;
int wait = 7000;
int initialTime = 0;
int deltaTime = 0;

char tempLabel[20] = "Temp:";
char counterLabel[20] = "Count:";
char payloadToSend[30];
String strToSend;

void setup()
{
  fixHardwareIssue();

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
  //get the temperature
  //float tempValue = getTemperature();

  char c = GPS.read();

  if(GPS.newNMEAreceived())
  {
    GPS.parse(GPS.lastNMEA());
  }
  
  if (millis() >= initialTime + wait)
  {
    initialTime = millis();
    int GPSTime = 60*60*GPS.hour + 60*GPS.minute + GPS.seconds;
    // Construct the string to send
    /* String(GPS.hour) +String(GPS.minute)+","+ String(GPS.seconds) + */
    strToSend = "time:" + String(GPSTime) + ",lat:" + String(GPS.latitude) + ",long:" + String(GPS.longitude);
    // Convert the string to a charactor array
    strToSend.toCharArray(payloadToSend, 30);
    // Send the payload
    loraSend(*payloadToSend, wait);
    counter++;
  }


  
}


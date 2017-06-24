#include <Sodaq_RN2483.h>
#include "Arduino.h"
#define debugSerial SerialUSB
#define loraSerial Serial2


const uint8_t devAddr[4] =
{
  0x8D,0x8D,0x12,0x40
};

// Set the Network session key to your value
const uint8_t nwkSKey[16] =
{
  0x6D,0xE7,0xB3,0x6A,0x89,0x6A,0x92,0x0D,0x03,0x00,0x15,0x10,0x6E,0x6B,0x8D,0xF7
};

//Set the Application session key to your value
const uint8_t appSKey[16] =
{
  0x39,0xFE,0x9F,0x73,0xE2,0x8A,0x51,0xF2,0x11,0x83,0xDB,0x77,0x4E,0xD6,0xEF,0x01
};

//Set up some values and labels to send
int counter=0 ;
char tempLabel[20]="Temp:";
char counterLabel[20]="Count:";
char payloadToSend[30];
String strToSend;

void setup()
{
  fixHardwareIssue();

  setLEDPins();

  startUpLEDFlash();

  connectLORA();
} 

void loop()
{
  // get the temperature
  float tempValue = getTemperature();
  // Construct the string to send
  strToSend = String(counterLabel) +String(counter)+","+ String(tempLabel) + String(tempValue);
  // Convert the string to a charactor array
  strToSend.toCharArray(payloadToSend, 30);
  // Send the payload
  loraSend(*payloadToSend);
  
  // Increment our counter
  counter++;
}


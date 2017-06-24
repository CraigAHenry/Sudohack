  //Some of the older boards had a hardware fault. Your boards should not be affected but just in case leave this code here
  //FIX HW ISSUE. STOP FLOATING PIN
void fixHardwareIssue()
{
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  delay(100);
  digitalWrite(4, HIGH);
}

  //At startup flash the LEDs Red, Green and Blue for one second each during startup three times and then turn the off



void startUpLEDFlash()
{
  for (int i = 0; i < 3 ; i ++)
  {
    RED();
    delay(100);
    GREEN();
    delay(100);
    BLUE();
    delay(100);
  }
  OFF();
}


void connectSerials()
{
    while ((!debugSerial) && (millis() < 10000));
    {
      debugSerial.begin(57600);
    }
    while ((!loraSerial) && (millis() < 10000));
    {
      loraSerial.begin(LoRaBee.getDefaultBaudRate());
    } 
      GPS.begin(9600);
}

void connectLORA()
{
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, false))
  {
    debugSerial.println("OTA Connection to the network was successful.");
      // flash the green LED 3 times if connection successful
      int connectflashcount=3;
      while (connectflashcount > 0)
      {
      GREEN();
      delay(200);
      OFF();
      delay(200);
      connectflashcount = connectflashcount-1;
      }
      
  }
  else
  {
    debugSerial.println("OTA Connection to the network failed!");
      // flash the red LED 10 times if connection failure
      int failureflashcount=10;
      while (failureflashcount > 0)
      {
      RED();
      delay(200);
      OFF();
      delay(200);
      failureflashcount = failureflashcount-1;
      }
  }
}

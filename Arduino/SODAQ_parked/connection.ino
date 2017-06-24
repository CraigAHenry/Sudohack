

void loraSend(char payLoadToSend, int wait)
{
  switch (LoRaBee.send(1, (uint8_t*) payloadToSend, 30))
   {
     case NoError:
      debugSerial.println(payloadToSend);
     debugSerial.println("Successful transmission.");
      // flash the blue LED 3 times if data sent OK
      BLUE();delay(200);OFF();delay(200);BLUE();delay(200);OFF();delay(200);BLUE();delay(200);OFF();delay(200);
      // wait 
     debugSerial.print("Waiting for ");
     debugSerial.print(wait);
     debugSerial.println(" ms.");
      break;
    case NoResponse:
     debugSerial.println("There was no response from the device...");
      break;
    case Timeout:
     debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
     debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
     debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe program will now halt.");
      while (1) {};
      break;
    case Busy:
     debugSerial.println("The device is busy. Doing nothing this time.");
      break;
    case NetworkFatalError:
     debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe program will now halt.");
      while (1) {};
      break;
    case NotConnected:
     debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe program will now halt.");
      while (1) {};
      break;
    case NoAcknowledgment:
     debugSerial.println("There was no acknowledgment sent back!");
      break;
    default:
      break;
  }
}

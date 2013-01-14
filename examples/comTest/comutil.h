#ifndef _comutil_h_
#define _comutil_h_

#if defined(ARDUINO)
#include "Arduino.h"
#else
#include "WProgram.h"
#define Serial SerialUSB
#endif
  
#include <commandInterpreter.h>

void printReply(unsigned char reply) {
  Serial.print("Received: ");
  if ( reply == ACK ) Serial.println("ACK");
  else if ( reply == CRC ) Serial.println("CRC");
  else if ( reply == NAK ) Serial.println("NAK");
  else if ( reply == CAN ) Serial.println("CAN");
  else if ( reply == UNK ) Serial.println("UNK");
  else Serial.println(reply);
}

#endif

        
        
        
        
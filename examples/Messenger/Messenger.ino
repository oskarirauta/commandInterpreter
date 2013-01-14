#include <commandInterpreter.h>
#include "comutil.h"
#include "handlers.h"

#define LED 13

unsigned char ledState = 0;

void action(void) {
  unsigned char b;

  if (( !COM.replyType ) || ( COM.isPublic )) // We haven't supported public commands
    return;

  while ( Serial1.available() != 0)
    Serial1.read();

  if ((COM.replyType == MSG) && ( COM.crcError ) && ( !COM.isPublic )) {
    if ( COM.isPublic ) return; // Do not answer to public messages with CRC's
    sendReply(COM.senderID, CRC);
    Serial.println("CRC error. Informing master.");
    return;
  }


  switch ( COM.command()[0] ) {
  case 'L': 
    {
      if ((COM.argCount > 0) && ( strcmp(COM.command(), "LED") == 0 )) {
        if (strcmp(COM.arg(0), "ON") == 0 ) { 
          digitalWrite(LED, HIGH); 
          ledState = 1; 
          sendReply(COM.senderID, ACK); 
        }
        else if (strcmp(COM.arg(0), "OFF") == 0 ) { 
          digitalWrite(LED, LOW); 
          ledState = 0; 
          sendReply(COM.senderID, ACK); 
        }
      }
      break;
    }
  case 'S': 
    {
      if ( strcmp(COM.command(), "STATE") == 0 ) {
        sendMSG(COM.senderID, "STATE", 1, ledState ? "ON" : "OFF");
        break;
      }
    }
  default: 
    sendReply(COM.senderID, UNK);
    break;
  }

}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(RTS, OUTPUT);
  digitalWrite(RTS, LOW);

  Serial.begin(115200);
  Serial1.begin(115200);
  delay(800); // Settle time - not possibly necessary..
}

void loop() {

  if ( Serial1.available() != 0 )
    while ( Serial1.available() != 0 )
      if ( COM.receive(Serial1.read()))
        action();

}




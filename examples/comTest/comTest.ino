#include <commandInterpreter.h>
#include "comutil.h"
#include "handlers.h"

void action(void) {
  unsigned char b;

  while ( Serial1.available() != 0)
    Serial1.read();

  if ( COM.crcError ) {
    if ( COM.isPublic ) return; // Do not answer to public messages with CRC's
    sendReply(COM.senderID, CRC);
    Serial.println("CRC error. Informing master.");
    return;
  }

  if ( !COM.replyType ) // We didn't receive it properly..
    return;

  if ( COM.replyType != MSG ) {
    printReply(COM.replyType);
    return;
  }

  Serial.print("Received ");
  if ( COM.isPublic ) Serial.print("public ");
  Serial.print(COM.command()); Serial.print("["); Serial.print(COM.argCount);  Serial.print("] ");

  for ( b = 0; b < COM.argCount; b++ ) {
    Serial.print(COM.arg(b)); Serial.print(" ");
  }
  Serial.println(" ");

  if ( !COM.isPublic )
    sendReply(COM.senderID, ACK);  
}

void setup() {
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


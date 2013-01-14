#ifndef _handlers_h_
#define _handlers_h_

#if defined(ARDUINO)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
  
#define RTS 9
// Timeout 2,5 seconds
#define REPLY_TIMEOUT 2500
#define MAX_RETRYS 1

commandInterpreter COM(10);

void _send(void) {
    digitalWrite(RTS, HIGH);
    Serial1.write(COM.composition, COM.compositionLen);
    Serial1.flush();
#if defined(ARDUINO)
    delayMicroseconds(92); // To ensure all the data is sent, before we change the data direction - for full speed, comment out..
#else
  	while ((USART1_BASE -> SR & USART_SR_TC ) == 0 );
#endif
    digitalWrite(RTS, LOW);
}

void sendReply(unsigned char devNo, unsigned char reply) {
    COM.reply(devNo, reply);
    _send();
}

unsigned char sendMSG(unsigned char devNo, const char* command, const unsigned char numArgs = 0, ... ) {
  unsigned long t1 ,roundRobinT = 0;
  unsigned char retrys = 0, gotAnswer = 0, answer = CRC;

  while ( answer == CRC ) {

    if ( retrys++ > MAX_RETRYS )
      return CRC;

    va_start(COM.valist, numArgs);
    COM.vcompose(devNo, command, numArgs);
    va_end(COM.valist);
    _send();

    // A work around because millis goes around in approx. 70 days..
    roundRobinT = millis();
    t1 = roundRobinT + REPLY_TIMEOUT;
    if ( t1 > roundRobinT ) roundRobinT = 0;

    if ( roundRobinT != 0 )
      while ( millis() > roundRobinT )
        if ( Serial1.available() != 0 )
          if (( gotAnswer = COM.receive(Serial1.read())))
            break;

    if ( !gotAnswer ) // We might have already got it while roundrobin timer was going on..
      while ( millis() < t1 )
        if ( Serial1.available() != 0 )
          if (( gotAnswer = COM.receive(Serial1.read())))
            break;

    answer = COM.replyType;
  }
  return answer;
}

#endif




        
        
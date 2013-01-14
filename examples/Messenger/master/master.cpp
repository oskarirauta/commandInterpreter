#ifndef RS485_COMMAND_DELAY
#define RS485_COMMAND_DELAY 22
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "include/RS485.h"
#include "../../../commandInterpreter.h"

#define REPLY_TIMEOUT 2500
#define MAX_RETRYS 3

RS485 rs485("/dev/tty.usbserial-FTVTL64A"); // Change this to your serial device..
commandInterpreter COM(1);

unsigned char sendMSG(unsigned char devNo, const char* command, const unsigned char numArgs = 0, ... ) {
  unsigned long t1;
  unsigned char retrys = 0, gotAnswer = 0, answer = CRC;

  while ( answer == CRC ) {

    if ( retrys++ > MAX_RETRYS )
      return CRC;

    va_start(COM.valist, numArgs);
    COM.vcompose(devNo, command, numArgs);
    va_end(COM.valist);
	rs485.sendBuf(COM.composition, COM.compositionLen);
    
    t1 = millis() + REPLY_TIMEOUT;

    while ( millis() < t1 )
      if ( rs485.available() != 0 )
        if (( gotAnswer = COM.receive(rs485.read())))
          break;

    answer = COM.replyType;
  }
  return answer;
}

void printReply(unsigned char reply) {
  if ( reply == MSG ) {
    printf("MSG: %s ", COM.command());
    
    unsigned char b;
    for ( b = 0; b < COM.argCount; b++ )
	printf("%s ", COM.arg(b));
    printf("\r\n");

    COM.reply(COM.senderID, ACK);
    rs485.sendBuf(COM.composition, COM.compositionLen);

  } else if ( reply == ACK ) printf("ACK\r\n");
    else if ( reply == NAK ) printf("NAK\r\n");
    else if ( reply == CAN ) printf("CAN\r\n");
    else if ( reply == UNK ) printf("UNK\r\n");
    else if ( reply == 0 ) printf("NONE\r\n");
    else printf("%d\r\n", reply);
}

void mssleep(int msdelay) {
	usleep(msdelay * 1000);
}

int main() {
	while (1) {
	mssleep(200);
      	printf("Asking LED state: ");
      	printReply(sendMSG(10, "STATE"));

	mssleep(3000);
      	printf("Sending LED ON: ");
      	printReply(sendMSG(10, "LED", 1, "ON"));

	mssleep(200);
        printf("Asking LED state: ");
      	printReply(sendMSG(10, "STATE"));
      
	mssleep(3000);
      	printf("Sending LED OFF: ");
      	printReply(sendMSG(10, "LED", 1, "OFF"));      
	}
}

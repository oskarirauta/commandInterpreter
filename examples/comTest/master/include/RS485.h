#ifndef __RS485_MASTER_H__
#define __RS485_MASTER_H__

/* RS485 implementation for master

written by Oskari Rauta
*/

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "wiringSerial.h"

#ifndef RS485_COMMAND_DELAY
#define RS485_COMMAND_DELAY 10
#endif
  
#ifndef RS485_OUTPUT_STR_MAXLEN
#define RS485_OUTPUT_STR_MAXLEN 500
#endif
  
double millis();
double micros();
  
class RS485 {
 private:
  int fd;
  double outputTimer;
  char outputBuf[RS485_OUTPUT_STR_MAXLEN];
  void delaySending(void);
  void _sendBuf(char *buf, short int size);
  void _sendStr(char *buf);
  void setTimer(void);
  void sendOutput(void);

 public:
  RS485(const char *devNode, const int baud=115200);
  void end(void);

  int available(void);
  int read(void);
  void flush(void);
  void write(const char *message, ...);
  int readChar(int timeout_ms);
  void _sendChar(char ch);
  void sendChar(const char ch);
  void sendBuf(const char *buf, short int size);
  void sendBuf(unsigned char *buf, short int size);
  void sendBuf(const char *buf);
  void sendStr(const char *buf);  
};
#endif

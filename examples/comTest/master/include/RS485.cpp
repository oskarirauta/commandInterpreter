/* RS485 implementation for master

written by Oskari Rauta
*/

#include "RS485.h"
  
double millis() {
  struct timeval  tv;
  gettimeofday(&tv, NULL);

  return ((tv.tv_sec) * 1000 ) + ((tv.tv_usec) / 1000 ) ;
}

double micros() {
  struct timeval  tv;
  gettimeofday(&tv, NULL);

  return ((tv.tv_sec) * 1000000 ) + tv.tv_usec;
}

RS485::RS485(const char *devNode, const int baud) {
  this->fd = serialOpen((char *)devNode, baud);
}

void RS485::end(void) {
  serialClose(this->fd);
}

void RS485::setTimer(void) {
  this->outputTimer = micros() + RS485_COMMAND_DELAY;
  
  double mNow = micros();
  double mNext = mNow + RS485_COMMAND_DELAY;
  
  if ( mNext < mNow ) { // Timer gone round-robin
    mNow--;
    while ( mNow < micros());
    mNext = micros() + RS485_COMMAND_DELAY;
  }
  this->outputTimer = mNext;
}

void RS485::sendOutput(void) {
  this->outputTimer = 0;
  while ( serialDataAvail(this->fd) > 0 )
    serialGetchar(this->fd);
  serialPuts(this->fd, outputBuf);
}

int RS485::available(void) {
  return serialDataAvail(this->fd);
}

int RS485::read(void) {
  if ( this->outputTimer > 0 )
    if ( micros() > this->outputTimer + RS485_COMMAND_DELAY )
      this->sendOutput();
  return serialGetchar(this->fd);
}

void RS485::flush(void) {
  serialFlush(this->fd);
}

void RS485::write(const char *message, ...) {
  va_list argp;
  char buffer[1024];
  
  va_start(argp, message);
  	vsnprintf(buffer, 1023, message, argp);
  va_end(argp);
  
  serialPuts(this->fd, buffer);
}

void RS485::_sendChar(char ch) {
  this->delaySending();
  while ( serialDataAvail(this->fd) > 0 )
    serialGetchar(this->fd);
  
  serialPutchar(this->fd, ch);
  usleep(1);
}

void RS485::_sendBuf(char *buf, short int size) {
  int i;
  this->delaySending();
  while ( serialDataAvail(this->fd) > 0 )
    serialGetchar(this->fd);
  for ( i = 0; i < size; i++ )
    serialPutchar(this->fd, buf[i]);
  usleep(4);  
}

void RS485::_sendStr(char *buf) {
  this->setTimer();
  strcpy(this->outputBuf, buf);
}

int RS485::readChar(int timeout_ms) {
//  return serialGetchar(this->fd);
  double _timer = millis();
  while ( _timer + timeout_ms > millis() )
    if ( serialDataAvail(this->fd) > 0 )
      return serialGetchar(this->fd);
    
  return -1;
}

void RS485::sendChar(const char ch) {
  this->_sendChar((char)ch);
}

void RS485::sendBuf(const char *buf, short int size) {
  this->_sendBuf((char *)buf, size);
}

void RS485::sendBuf(unsigned char *buf, short int size) {
  int i;
  this->delaySending();
  while ( serialDataAvail(this->fd) > 0 )
    serialGetchar(this->fd);
  for ( i = 0; i < size; i++ )
    serialPutchar(this->fd, buf[i]);
  usleep(4);
}

void RS485::sendBuf(const char *buf) {
  this->_sendBuf((char *)buf, strlen(buf));
}

void RS485::sendStr(const char *buf) {
  this->_sendStr((char *)buf);
}

void RS485::delaySending(void) {
  usleep(RS485_COMMAND_DELAY);
}

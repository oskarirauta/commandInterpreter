/* commandInterpreter v2.0 library

written by Oskari Rauta
*/

#include "commandInterpreter.h"

commandInterpreter::commandInterpreter(const unsigned char devID) {
      this->listHead = NULL;
      this->listTail = NULL;
      this->myID = devID;
      this->last2[0] = 0;
      this->last2[1] = 0;
      this->crcError = 0;
      this->compositionLen = 0;
      this->replyType = 0;
      this->isPublic = 0;
      this->argCount = 0;
      this->senderID = 0;
      this->receiverID = 0;
      this->crc = 0;
      this->receivingHeader = 0;
}

void commandInterpreter::compose(unsigned char receiver, const char *command, const unsigned char numArgs, ... ) {
  if ( numArgs > 0 ) {
        va_start(this->valist, numArgs);
  		this->vcompose(receiver, command, numArgs);
    	va_end(this->valist);
  } else this->vcompose(receiver, command, numArgs);
}

void commandInterpreter::vcompose(unsigned char receiver, const char *command, const unsigned char numArgs) {
  unsigned short sl, i = 15;

  this->reset();
  this->crc = 175; // chksum for 6x beginTX(ascii 29) + 1 is 175
  memset(this->composition, 0, 2);
  memset(this->composition + 2, 29, 6 );
  this->composition[8] = 01;
  this->composition[9] = 107 + numArgs;
  this->composition[10] = 48 + this->myID;
  this->composition[11] = 48 + receiver;
  /* composition[12] and composition[13] reserved for checksum, initially they are zeroed. */
  memset(this->composition + 12, 0, 2);
  this->composition[14] = 02;
  
  sl = strlen(command);
  memcpy(this->composition + i, command, sl);
  this->composition[i+sl] = 30;
  i += (sl + 1);
  
  if ( numArgs > 0 ) {
    unsigned char b;
    
    for ( b = 0; b < numArgs; b++ ) {
      char *arg = va_arg(valist, char *);
      sl = strlen(arg);
      memcpy(this->composition + i, arg, sl);
      this->composition[i+sl] = 30;
      i += (sl + 1);
    }
  }
  
  this->composition[i++] = 3;
  this->composition[i++] = 0;
  this->composition[i++] = 0;
  
  for ( sl = 9; sl < ( i - 2 ); sl++ )
    this->crc += this->composition[sl];
  
  if ( this->crc == 0 ) this->crc = 128; // Do not send "endtx"..
  
  memset(this->composition + 12, this->crc, 2);
  this->compositionLen = sl+1; // If no args is included and command length is only 1 byte, 17 bytes packet is received by parser
}

void commandInterpreter::reply(unsigned char receiver, unsigned char reply ) {
  unsigned char b;
  
  this->reset();
  this->crc = 175; // chksum for 6x beginTX(ascii 29) + 1 is 175
  memset(this->composition, 0, 2);
  memset(this->composition + 2, 29, 6 );
  this->composition[8] = 01;
  this->composition[9] = 107; // 0 number of args for reply
  this->composition[10] = 48 + this->myID;
  this->composition[11] = 48 + receiver;
  /* composition[12] and composition[13] reserved for checksum, initially they are zeroed. */
  memset(this->composition + 12, 0, 2);
  this->composition[14] = 31;
  this->composition[15] = reply;
  this->composition[16] = 30;
  this->composition[17] = 3;
  memset(this->composition + 18, 0, 2);

  for ( b = 9; b < 19; b ++)
    this->crc += this->composition[b];
  
  if ( this->crc == 0 ) this->crc = 128; // Do not send "endtx"..
  
  memset(this->composition + 12, this->crc, 2);
  this->compositionLen = 20; // Reply is always 20 bytes, 16 bytes when received..
}

bool commandInterpreter::parse(void) {
  unsigned char checksum, _argCount = 0;
  unsigned short sl, argLen = 0;

  if (( this->compositionLen < 16) || // Packet contains no data
      (( this->composition[12] != 2 ) && ( this->composition[12] != 31 )) || // Wrong format, no message, no reply..
      (( this->composition[9] - 48 != this->myID ) && ( this->composition[9] != 251 ))) { // We are not the receiver of packet
    this->reset();
    return false;
  }

  this->argCount = this->composition[7] - 107;
  this->senderID = this->composition[8] - 48;
  this->receiverID = this->myID;
  this->isPublic = ( this->composition[9] == 251 ? true : false );  
  
  if ( this->composition[12] == 31 ) { // We are receiving a reply instead of a message
    this->replyType = this->composition[13];
    
    if (( this->replyType != ACK ) && ( this->replyType != NAK ) && ( this->replyType != CAN ) &&
        ( this->replyType != UNK ) && ( this->replyType != CRC )) // We got a unknown reply type
      this->crcError = true;
  } else if ( this->composition[12] == 2 ) this->replyType = MSG;
  
  checksum = this->composition[10];
  if ( this->composition[10] != this->composition[11] ) { // Checksum mismatch or wrong format
    this->crcError = true; 
    return true;
  }

  this->composition[10] = this->composition[11] = 0; // Clear original checksum bytes for checksum checking
  this->crc = 0; // Start from 0..
  for ( sl = 0; sl < compositionLen; sl++ ) this->crc += this->composition[sl];
  this->composition[10] = this->composition[11] = checksum; // Return the data to the way it was
  
  if ( this->crc == 0 ) this->crc = 128; // our workaround for not sending 0's..
 
  if ( checksum != this->crc ) this->crcError = true; // Checksum mismatch
  
  if ( this->replyType == 0 ) { this->reset(); return false; } // Format not identified
  if ( this->crcError ) return true; // Do not go to parsing if we have a mismatch

  if ( this->replyType != MSG ) return true; // Reply parsing ends here..
    
  // Now, everything seems to be in order and we seem to be receiving a message. Parse arguments to our list.  
  if ( this->listHead != NULL )
    this->clearList(); // Results of previous parsing was still there. Get rid of it..

  unsigned short b = 13;
  for ( sl = 13; sl < compositionLen - 1; sl++ )
    if ( composition[sl] == 30 ) {
      arg_item *node = NULL;
      node = (arg_item *)malloc(sizeof(arg_item));
      node->data = (char *)malloc(argLen+1);
      memcpy(node->data, composition + b, argLen);
      node->data[argLen] = '\0';
      node->next = NULL;
      if ( this->listHead == NULL ) {
        this->listHead = node;
        this->listTail = node;
      } else {
        this->listTail->next = node;
        this->listTail = node;
      }
      b = sl + 1;
      argLen = 0;
      _argCount++;
    } else argLen++;

  if ( --_argCount != this->argCount ) crcError = true;  // Checksum mismatch
  else this->replyType = MSG;

  return true;
}

void commandInterpreter::clearList(void) {
  if ( this->listHead == NULL )
    return;
  this->listTail = NULL;
  while ( this->listHead != NULL ) {
    arg_item *node = this->listHead;
    listHead = node -> next;
    node -> next = NULL;
    free(node->data);
    node->data = NULL;
    free(node);
    node = NULL;
  }
}

void commandInterpreter::reset(void) {
  this->crcError = 0;
  this->compositionLen = 0;
  this->replyType = 0;
  this->isPublic = 0;
  this->argCount = 0;
  this->senderID = 0;
  this->receiverID = 0;
  memset(this->last2, 0, 2);
  this->receivingHeader = 0;
  this->clearList();
}

bool commandInterpreter::receive(unsigned char ch) {
  // Check if we got our packet header + end command/arg + endtext + endtx
  if (( ch == 0 ) && ( this->receivingHeader == 10) && ( this->last2[0] == 30 ) && ( this->last2[1] == 3 )) {
    return this->parse();
  }

  this->last2[0] = this->last2[1]; this->last2[1] = ch;
  
  if ( ch == 0 ) { this->reset(); this->receivingHeader = 1; return false; }
  if (( ch == 29 ) && ( this->receivingHeader > 0 ) && ( this->receivingHeader != 10 )) {
    this->receivingHeader++;
    if ( this->receivingHeader == 7 ) { // Packeteader received as complete..
      this->receivingHeader = 10;
      memset(this->composition, 29, 6);
      this->compositionLen = 6;
    }
    return false;
  }
  
  if ( this->receivingHeader != 10 ) return false; // We are waiting for packet header {0, 29, 29, 29, 29, 29, 29}

  if ( this->compositionLen > MAX_COMPOSITION_LENGTH ) {
    this->reset();
    return false;
  }
  
  this->composition[this->compositionLen++] = ch;
  return false;
}

char *commandInterpreter::command(void) {
  return this->listHead->data;
}

char *commandInterpreter::arg(unsigned char argNum) {
  if ( argNum > ( this->argCount - 1 ))
    return NULL;
  unsigned char b;
  arg_item *node = this->listHead->next;
  for ( b = 0; b < argNum; b++ )
    node = node->next;
  return node->data;
}

unsigned char commandInterpreter::deviceID(void) {
  return this->myID;
}

void commandInterpreter::setDeviceID(unsigned char newID) {
  this->myID = newID;
}

        
        
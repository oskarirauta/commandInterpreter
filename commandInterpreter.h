#ifndef __COMMANDINTERPRETER_H__
#define __COMMANDINTERPRETER_H__
  
#ifndef MAX_COMPOSITION_LENGTH
	#define MAX_COMPOSITION_LENGTH 640
#endif

#define ALLRECEIVERS 203

#define MSG 1
#define ACK 6
#define CRC 8
#define NAK 21
#define CAN 24
#define UNK 27
  
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef struct item_element {
  struct item_element *next;
  char *data;
} arg_item;

class commandInterpreter {
  public:
    commandInterpreter(const unsigned char devID = 99);
    unsigned char composition[MAX_COMPOSITION_LENGTH + 6];
    unsigned char crcError; /* bool */
    unsigned short compositionLen;
    unsigned char replyType;
    unsigned char isPublic; /* bool */
    unsigned char argCount;
    unsigned char senderID;
    unsigned char receiverID;
    unsigned char crc;
  	va_list valist;
    
    void compose(unsigned char receiver, const char *command, const unsigned char numArgs = 0, ... );
    void vcompose(unsigned char receiver, const char *command, const unsigned char numArgs = 0);
    void reply(unsigned char receiver, unsigned char reply);
    void reset(void);
    bool receive(unsigned char ch);
    char *command(void);
    char *arg(unsigned char argNum);
    unsigned char deviceID(void);
    void setDeviceID(unsigned char newID);
  
  private:
    unsigned char last2[2];
    unsigned char receivingHeader;
    arg_item *listHead, *listTail;  
    unsigned char myID;
    
    bool parse(void);
    void clearList(void);
};
  
#endif

        
        
        
#include "plc/plc_message.h"

#include <stdlib.h>

typedef struct PlcMessage_t {
  int a;
} PlcMessage_t;


PlcMessage_t* createPlcMessage(){
  return malloc(sizeof(PlcMessage_t));
}

int setAPDU(PlcMessage_t* msg, XwayAPDUCode_e code, uint8_t* data, int dataLen){
  return 0;
}

int setNPDU(PlcMessage_t* msg, XwayNPDUType_e code, XwayAddr sender, XwayAddr receiver, uint8_t* extendedAddr){
  return 0;
}

XwayAddr createXwayAddr(uint8_t station, uint8_t network, uint8_t port){
  return 0;
}

// returns the msg size
size_t serializePlcMessage_t(PlcMessage_t* msg, uint8_t* serMsg){
  return 0;
}

// returns the msg
PlcMessage_t* deserializePlcMessage_t( uint8_t* serMsg){
  return 0;
}

int freeMessage(PlcMessage_t* msg){
  return 0;
}

#ifndef PLC_INFO_H_
#define PLC_INFO_H_
#include "plc/model_info.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct PlcMessage_t PlcMessage_t;

typedef uint16_t XwayAddr;

typedef enum {
  APDU_WRITE_RESP = 0xFE,
  APDU_WRITE_REQ = 0x37
}XwayAPDUCode_e;

typedef enum {
  NPDU_3WAY = 0xF0,
  NPDU_5WAY = 0xF1,
}XwayNPDUType_e;

PlcMessage_t* createPlcMessage();

int setAPDU(PlcMessage_t* msg, XwayAPDUCode_e code, uint8_t* data, int dataLen);

int setNPDU(PlcMessage_t* msg, XwayNPDUType_e code, XwayAddr sender, XwayAddr receiver, uint8_t* extendedAddr);

XwayAddr createXwayAddr(int8_t station, int8_t network, int8_t port);

// returns the msg size
size_t serializePlcMessage_t(PlcMessage_t* msg, uint8_t* serMsg);

// returns the msg
PlcMessage_t* deserializePlcMessage_t( uint8_t* serMsg, size_t msgSize);

int freeMessage(PlcMessage_t* msg);

#endif  // PLC_INFO_H_

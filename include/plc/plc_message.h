#ifndef PLC_INFO_H_
#define PLC_INFO_H_
#include "plc/model_info.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_MSG_SIZE 28

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

typedef enum {
  PLC_CATEGORY = 0x06,
  PC_CATEGORY = 0x07
}XwayMachineCategory_e;

typedef enum {
  BIT_SPACE = 0x64,
  INTERNAL_DATA_SPACE = 0x68,
  CONSTANT_DATA_SPACE = 0x69,
  SYSTEM_DATA_SPACE = 0x6A
}UniteObjectSegments_e;

typedef enum {
  UNITE_TYPE_DATE = 1,
  UNITE_TYPE_WORD = 7,
  UNITE_TYPE_DOUBLE = 8,
}UniteObjectType_e;

PlcMessage_t* createPlcMessage();

PlcMessage_t* createACK(PlcMessage_t* msg, bool res);

int setAPDU(PlcMessage_t* msg, XwayAPDUCode_e code, uint8_t* data, int dataLen);

int setNPDU(PlcMessage_t* msg, XwayNPDUType_e code, XwayAddr sender, XwayAddr receiver, uint8_t* extendedAddr);

XwayAddr createXwayAddr(uint8_t station, uint8_t network, uint8_t port);

// returns the msg size
size_t serializePlcMessage(PlcMessage_t* msg, uint8_t* serMsg);

// returns the msg
PlcMessage_t* deserializePlcMessage( uint8_t* serMsg);

int freeMessage(PlcMessage_t* msg);

#endif  // PLC_INFO_H_

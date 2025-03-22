#include "plc/plc_facade.h"
#include <byteswap.h>
#include <string.h>
#include <stdint.h>

int configWritePlcMessage(PlcMessage_t* msg, PlcMessageType_e msgType,
                          uint8_t station, enum TrainId_e trainId, int target){
  uint8_t data[13];
  memset(data, 0, sizeof(data));
  int dataLen = 13;

  data[0] = (uint8_t) PLC_CATEGORY;
  data[1] = (uint8_t) INTERNAL_DATA_SPACE;
  data[2] = (uint8_t) UNITE_TYPE_WORD;

  uint16_t trainAddr = getTrainAddr(trainId);
  memcpy(data+3, &trainAddr, sizeof(uint16_t)); // implicitly swaps byte order

  uint16_t len = 3;
  memcpy(data+5, &len, sizeof(uint16_t));// implicitly swaps byte order

  memcpy(data+7, &station, sizeof(uint16_t));// implicitly swaps byte order

  data[9] = 0xFF;
  data[10] = 0xFF;
  data[11] = 0xFF;
  data[12] = 0xFF;

  switch(msgType){
    case(TOGGLE_RAIL):
      memcpy(data+9, &target, sizeof(uint16_t)); // implicitly swaps byte order
      break;

    case (TOGGLE_SWITCH):
      memcpy(data+11, &target, sizeof(uint16_t)); // implicitly swaps byte order
      break;
  }

  int res = setAPDU(msg, APDU_WRITE_REQ, data, dataLen);
  return res;
}

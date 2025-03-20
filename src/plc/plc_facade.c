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
  trainAddr = bswap_16(trainAddr);

  memcpy(data+3, &trainAddr, sizeof(uint16_t));

  uint16_t len = 3;
  len = bswap_16(len);
  memcpy(data+5, &len, sizeof(uint16_t));

  station = bswap_16((uint16_t) station);
  memcpy(data+7, &station, sizeof(uint16_t));

  data[9] = 0xFF;
  data[10] = 0xFF;
  data[11] = 0xFF;
  data[12] = 0xFF;

  switch(msgType){
    case(TOGGLE_RAIL):
      uint16_t railId = bswap_16(getRailAddr(trainId,target));
      memcpy(data+9, &railId, sizeof(uint16_t));
      break;

    case (TOGGLE_TRAIN_DIR):
      uint16_t inverterId = bswap_16(getInverterAddr(trainId,target));
      memcpy(data+9, &inverterId, sizeof(uint16_t));
      break;

    case (TOGGLE_SWITCH):
      uint16_t switchId = bswap_16(getSwitchAddr(trainId,target));
      memcpy(data+11, &switchId, sizeof(uint16_t));
      break;

  }

  int res = setAPDU(msg, APDU_WRITE_REQ, data, dataLen);
  return res;
}

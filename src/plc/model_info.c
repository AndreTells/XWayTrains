#include "plc/model_info.h"
#include <stdint.h>
#include <string.h>


static const uint16_t TrainAddrDict[] = {
    [TRAIN_1] = 0x0027,
    [TRAIN_2] = 0x002A,
    [TRAIN_3] = 0x0032,
    [TRAIN_4] = 0x0035,
};

uint16_t getTrainAddr(TrainId_e trainId){
    return TrainAddrDict[trainId];
}

static const TrainId_e AckAddrDict [] = {
  [ADDR_ACK_RAIL_1] =  TRAIN_1,
  [ADDR_ACK_SWITCH_1] = TRAIN_1,

  [ADDR_ACK_RAIL_2] =   TRAIN_2,
  [ADDR_ACK_SWITCH_2] = TRAIN_2,

  [ADDR_ACK_RAIL_3] =  TRAIN_3,
  [ADDR_ACK_SWITCH_3] =  TRAIN_3,

  [ADDR_ACK_RAIL_4] =  TRAIN_4,
  [ADDR_ACK_SWITCH_4] = TRAIN_4,
  [ADDR_ACK_INV_4] =  TRAIN_4,
};


TrainId_e getTargetedTrain(uint8_t* writeData){
    uint16_t writeAddr;
    memcpy(&writeAddr, writeData+3, sizeof(uint16_t)); // implicitly swaps byte order

    return AckAddrDict[writeAddr];
}

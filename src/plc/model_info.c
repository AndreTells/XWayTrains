#include "plc/model_info.h"
#include <stdint.h>


static const uint16_t TrainAddrDict[] = {
    [TRAIN_1] = 0x0027,
    [TRAIN_2] = 0x002A,
    [TRAIN_3] = 0x0032,
    [TRAIN_4] = 0x0035,
};

uint16_t getTrainAddr(TrainId_e trainId){
    return TrainAddrDict[trainId];
}


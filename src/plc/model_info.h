#ifndef MODEL_INFO_H_
#define MODEL_INFO_H_
#include <stdint.h>

uint16_t getTrainAddr(TrainId_e trainId);

uint16_t getRailAddr(TrainId_e trainId, int railId);

uint16_t getSwitchAddr(TrainId_e trainId, int railId);

uint16_t getInverterAddr(TrainId_e trainId, int railId);

#endif // MODEL_INFO_H_

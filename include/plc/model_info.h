/**
 * @file model_info.h
 * @brief Definitions of identifiers for trains, rails, switches, and reversible
 * rails
 * @details Contains enumerations for identifying trains and various rail
 * components in the physical train model.
 *          TODO(andre): asign address to all the enum values
 */

#ifndef MODEL_INFO_H_
#define MODEL_INFO_H_
#include <stdint.h>

/**
 * @brief Enumeration of train identifiers
 */
typedef enum TrainId_e {UNKNOWN_TRAIN, TRAIN_1, TRAIN_2, TRAIN_3, TRAIN_4 }TrainId_e;

uint16_t getTrainAddr(TrainId_e trainId);

uint16_t getRailAddr(TrainId_e trainId, int railId);

uint16_t getSwitchAddr(TrainId_e trainId, int railId);

uint16_t getInverterAddr(TrainId_e trainId, int railId);

#endif  // MODEL_INFO_H_

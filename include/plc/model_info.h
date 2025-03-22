/**
 * @file model_info.h
 * @brief Definitions of identifiers for trains, rails, switches, and reversible
 * rails
 * @details Contains enumerations for identifying trains and various rail
 * components in the physical train model as well as addresses configured in the
 * plc
 */

#ifndef MODEL_INFO_H_
#define MODEL_INFO_H_
#include <stdint.h>

/**
 * @brief Enumeration of train identifiers
 */
typedef enum TrainId_e {
  UNKNOWN_TRAIN, TRAIN_1, TRAIN_2, TRAIN_3, TRAIN_4
}TrainId_e;

typedef enum RailId_e {
  RAIL_Tn0   = 0 , RAIL_Tn01   = 1 , RAIL_Tn02   = 2 , RAIL_Tn03   = 3 ,
  RAIL_Tn04  = 4 , RAIL_Tn07h  = 7 , RAIL_Tn07t  = 37, RAIL_Tn09   = 9 ,
  RAIL_Tn10  = 10, RAIL_T12    = 12, RAIL_T13    = 13, RAIL_T15    = 15,
  RAIL_T19   = 19, RAIL_T20    = 20, RAIL_T22    = 22, RAIL_T23    = 23,
  RAIL_T24   = 24, RAIL_T26    = 26, RAIL_T27    = 27, RAIL_T28    = 28,
  RAIL_T29   = 29, RAIL_T30    = 30, INV         = 47
} RailId_e;
// obs: inv targets both Ti07 and Ti09

typedef enum SwitchGroupId_e {
  SWITCH_GROUP_0 = 0,   // A0d
  SWITCH_GROUP_1 = 1,   // A1d
  SWITCH_GROUP_3 = 3,   // PA3b, A11b A7d
  SWITCH_GROUP_7 = 7,   // A7d, A11b, PA3d
  SWITCH_GROUP_10 = 10, // A10d, A9b, a8b, A7b, A11b, PA3b
  SWITCH_GROUP_12 = 12, // A12d
  SWITCH_GROUP_13 = 13, // A13b, A13d, Tj3d, A15b
  SWITCH_GROUP_14 = 14, // A14, Tj2d, A12b
  SWITCH_GROUP_20 = 20, // PA0d, A5d, A6d
  SWITCH_GROUP_21 = 21, // PA1d
  SWITCH_GROUP_22 = 22, // PA2d, Tj2d
  SWITCH_GROUP_23 = 23, // PA3d, A11b, A7b, A8b, A9b, A10d
  SWITCH_GROUP_31 = 31, // Tj1d, PA0d
  SWITCH_GROUP_33 = 33, // Tj3b, Tj3d
}SwitchGroupId_e;

typedef enum ACKAddr_e {
  ADDR_ACK_RAIL_1 = 8,
  ADDR_ACK_SWITCH_1 = 9,

  ADDR_ACK_RAIL_2 = 15,
  ADDR_ACK_SWITCH_2 = 16,

  ADDR_ACK_RAIL_3 = 10,
  ADDR_ACK_SWITCH_3 = 11,

  ADDR_ACK_RAIL_4 = 12,
  ADDR_ACK_SWITCH_4 = 13,
  ADDR_ACK_INV_4 = 14,
}ACKAddr_e;

uint16_t getTrainAddr(TrainId_e trainId);

TrainId_e getTargetedTrain(uint8_t* writeData);

#endif  // MODEL_INFO_H_

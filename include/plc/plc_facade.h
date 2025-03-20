/**
 * @file plc_facade.h
 * @brief Interface for creating PLC messages to control train-related
 * components
 * @details Provides functions to generate messages for toggling rail states,
 *          switch states, and inverting rail directions.
 */
#ifndef PLC_FACADE_H_
#define PLC_FACADE_H_
#include <stdbool.h>

#include "plc/model_info.h"
#include "plc/plc_message.h"

typedef enum {
  TOGGLE_RAIL,
  TOGGLE_SWITCH,
  TOGGLE_TRAIN_DIR
}PlcMessageType_e;

int configWritePlcMessage(PlcMessage_t* msg, PlcMessageType_e msgType,
                          uint8_t station, enum TrainId_e trainId, int target);

#endif  // PLC_FACADE_H_

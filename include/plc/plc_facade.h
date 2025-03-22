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

/**
 * @enum PlcMessageType_e
 * @brief Enumeration of PLC message types.
 *
 * This enumeration specifies the types of messages that can be generated for the PLC.
 */
typedef enum {
  TOGGLE_RAIL,
  TOGGLE_SWITCH,
}PlcMessageType_e;

/**
 * @brief Configures a PLC message for writing.
 *
 * This function sets up a PLC message with the specified type, station, train identifier, and target.
 *
 * @param msg Pointer to the PlcMessage_t structure that will be configured.
 * @param msgType The type of message to be configured (e.g., TOGGLE_RAIL or TOGGLE_SWITCH).
 * @param station The station identifier where the message will be applied.
 * @param trainId The train identifier associated with the message.
 * @param target The target component (e.g., rail or switch) to be toggled.
 * @return int Status code (0 for success, negative for error).
 */

int configWritePlcMessage(PlcMessage_t* msg, PlcMessageType_e msgType,
                          uint8_t station, enum TrainId_e trainId, int target);

#endif  // PLC_FACADE_H_

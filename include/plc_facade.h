/**
 * @file plc_facade.h
 * @brief Interface for creating PLC messages to control train-related
 * components
 * @details Provides functions to generate messages for toggling rail states,
 *          switch states, and inverting rail directions.
 */
#ifndef PLC_FACADE_H_
#define PLC_FACADE_H_

#include "model_info.h"
#include "plc_info.h"

/**
 * @brief Create a message to toggle the state of a powered rail
 * @param[out] msg Pointer to the PlcMessage_t structure to populate
 * @param[in] trainId Identifier of the train (from the TrainId_e enum)
 * @param[in] railId Identifier of the powered rail (from the PoweredRailId_e
 * enum)
 * @return 0 on success, non-zero error code on failure
 */
int createToggleRailStateMessage(PlcMessage_t* msg, enum TrainId_e trainId,
                                 enum PoweredRailId_e railId);

/**
 * @brief Create a message to toggle the state of a switch
 * @param[out] msg Pointer to the PlcMessage_t structure to populate
 * @param[in] trainId Identifier of the train (from the TrainId_e enum)
 * @param[in] railId Identifier of the switch (from the SwitchId_e enum)
 * @return 0 on success, non-zero error code on failure
 */
int createToggleSwitchStateMessage(PlcMessage_t* msg, enum TrainId_e trainId,
                                   enum SwitchId_e railId);

/**
 * @brief Create a message to invert the direction of a reversible rail
 * @param[out] msg Pointer to the PlcMessage_t structure to populate
 * @param[in] trainId Identifier of the train (from the TrainId_e enum)
 * @param[in] railId Identifier of the reversible rail (from the
 * InversibleRailId_e enum)
 * @return 0 on success, non-zero error code on failure
 */
int createInvertDirectionMessage(PlcMessage_t* msg, enum TrainId_e trainId,
                                 enum InversibleRailId_e railId);

#endif  // PLC_FACADE_H_

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

/**
 * @brief Enumeration of train identifiers
 */
typedef enum TrainId_e {UNKNOWN_TRAIN, TRAIN_1, TRAIN_2, TRAIN_3, TRAIN_4 }TrainId_e;

/**
 * @brief Enumeration of rail identifiers
 * @details Contains all constants regarding the physical train model rails.
 */
typedef enum RailId_e {
  UNKNOWN_RAIL,
  RAIL1,
  // TODO(andre): chechk ammount of rail Ids
}RailId_e;

/**
 * @brief Enumeration of powered rail identifiers
 */
typedef enum PoweredRailId_e {
  P_RAIL1,
  // TODO(andre): chechk ammount of rail Ids
}PoweredRailId_e;

/**
 * @brief Enumeration of switch identifiers
 */
typedef enum SwitchId_e {
  SWITCH1,
  // TODO(andre): chechk ammount of rail Ids
}SwitchId_e;

/**
 * @brief Enumeration of reversible rail identifiers
 */
typedef enum InversibleRailId_e {
  I_RailId1,
  // TODO(andre): chechk ammount of rail Ids
}InversibleRailId_e;

#endif  // MODEL_INFO_H_

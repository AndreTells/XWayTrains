/**
 * @file model_info.h
 * @brief Definitions of identifiers for trains, rails, switches, and reversible rails
 * @details Contains enumerations for identifying trains and various rail components
 *          in the physical train model.
 *          TODO(andre): asign address to all the enum values
 */

#ifndef MODEL_INFO_H_
#define MODEL_INFO_H_

/**
 * @brief Enumeration of train identifiers
 */
enum TrainId_e {
  TRAIN_1,
  TRAIN_2,
  TRAIN_3,
  TRAIN_4
};


/**
 * @brief Enumeration of rail identifiers
 * @details Contains all constants regarding the physical train model rails.
 */
enum RailId_e{
  UNKOWN_RAIL,
  RAIL1,
  //TODO(andre): chechk ammount of rail Ids
};

/**
 * @brief Enumeration of powered rail identifiers
 */
enum PoweredRailId_e{
  P_RAIL1,
  //TODO(andre): chechk ammount of rail Ids
};

/**
 * @brief Enumeration of switch identifiers
 */
enum SwitchId_e{
  SWITCH1,
  //TODO(andre): chechk ammount of rail Ids
};

/**
 * @brief Enumeration of reversible rail identifiers
 */
enum InversibleRailId_e{
  I_RailId1,
  //TODO(andre): chechk ammount of rail Ids
};

#endif // MODEL_INFO_H_

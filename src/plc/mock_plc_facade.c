#include "plc/plc_facade.h"


int createToggleRailStateMessage(PlcMessage_t* msg, enum TrainId_e trainId,
                                 enum PoweredRailId_e railId){
  msg = getNullMessage();
  return 0;
}

int createToggleSwitchStateMessage(PlcMessage_t* msg, enum TrainId_e trainId,
                                   enum SwitchId_e railId){
  msg = getNullMessage();
  return 0;
}


int createInvertDirectionMessage(PlcMessage_t* msg, enum TrainId_e trainId,
                                 enum InversibleRailId_e railId){
  msg = getNullMessage();
  return 0;
}

#include "plc/plc_facade.h"

int configWritePlcMessage([[maybe_unused]] PlcMessage_t* msg,
                          [[maybe_unused]] PlcMessageType_e msgType,
                          [[maybe_unused]] uint16_t station,
                          [[maybe_unused]] enum TrainId_e trainId,
                          [[maybe_unused]] int target) {
  return 0;
}

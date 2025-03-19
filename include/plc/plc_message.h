#ifndef PLC_INFO_H_
#define PLC_INFO_H_
#include <stdlib.h>

typedef enum {
  TOGGLE_RAIL,
  TOGGLE_SWITCH,
  TOGGLE_TRAIN_DIR
}PlcMessageType_e;

// TODO(andre): replace with actual Plc Message format
typedef struct PlcMessage_t PlcMessage_t;

PlcMessage_t* getNullMessage(void);

size_t getMessageSize(PlcMessage_t* msg);

int freeMessage(PlcMessage_t* msg);
#endif  // PLC_INFO_H_

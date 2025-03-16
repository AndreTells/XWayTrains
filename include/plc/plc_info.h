#ifndef PLC_INFO_H_
#define PLC_INFO_H_
#include <stdlib.h>

// TODO(andre): replace with actual Plc Message format
typedef struct PlcMessage_t PlcMessage_t;

PlcMessage_t* getNullMessage(void);

size_t getMessageSize(PlcMessage_t* msg);

int freeMessage(PlcMessage_t* msg);
#endif  // PLC_INFO_H_

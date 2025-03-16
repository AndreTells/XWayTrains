#include "plc/plc_info.h"

#include <stdlib.h>

typedef struct PlcMessage_t {
  int a;
} PlcMessage_t;

PlcMessage_t* getNullMessage(void) {
  PlcMessage_t* msg = (PlcMessage_t*)malloc(sizeof(PlcMessage_t));
  msg->a = 0;
  return msg;
}

size_t getMessageSize(PlcMessage_t* msg) { return sizeof(PlcMessage_t); }

int freeMessage(PlcMessage_t* msg) {
  (void)free(msg);
  return 0;
}

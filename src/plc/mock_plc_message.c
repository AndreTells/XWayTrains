#include <stdlib.h>

#include "plc/plc_message.h"

typedef struct PlcMessage_t {
  int a;
} PlcMessage_t;

PlcMessage_t* createPlcMessage() { return malloc(sizeof(PlcMessage_t)); }

int setAPDU([[maybe_unused]] PlcMessage_t* msg,
            [[maybe_unused]] XwayAPDUCode_e code,
            [[maybe_unused]] uint8_t* data, [[maybe_unused]] int dataLen) {
  return 0;
}

int setNPDU([[maybe_unused]] PlcMessage_t* msg,
            [[maybe_unused]] XwayNPDUType_e code,
            [[maybe_unused]] XwayAddr sender,
            [[maybe_unused]] XwayAddr receiver,
            [[maybe_unused]] uint8_t* extendedAddr) {
  return 0;
}

XwayAddr createXwayAddr([[maybe_unused]] uint8_t station,
                        [[maybe_unused]] uint8_t network,
                        [[maybe_unused]] uint8_t port) {
  return 0;
}

// returns the msg size
size_t serializePlcMessage_t([[maybe_unused]] PlcMessage_t* msg,
                             [[maybe_unused]] uint8_t* serMsg) {
  return 0;
}

// returns the msg
PlcMessage_t* deserializePlcMessage_t([[maybe_unused]] uint8_t* serMsg) {
  return 0;
}

int freeMessage([[maybe_unused]] PlcMessage_t* msg) { return 0; }

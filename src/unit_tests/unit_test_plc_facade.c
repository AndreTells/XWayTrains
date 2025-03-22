#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "plc/plc_facade.h"
#include "plc/plc_message.h"
#include "plc/model_info.h"
#include "common/verbose.h"
#include "common/flags.h"

void test_configWritePlcMessage_toggleSwitch() {
  verbose("[Plc Facade] configWritePlcMessage (TOGGLE_SWITCH) ... \n");

  /* Create a new message */
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  int res;
  uint8_t pc_station = 0x28;
  uint8_t plc_station = 0x0E;

  uint8_t network = 1;
  uint8_t port = 0;

   res = configWritePlcMessage(msg, TOGGLE_SWITCH, pc_station , TRAIN_1, SWITCH_GROUP_31);
  assert(res == 0);

  /* configure the address information */
  XwayAddr sender = createXwayAddr(pc_station, network, port);
  XwayAddr receiver = createXwayAddr(plc_station, network, port);
  uint8_t extAddr[2] = {0x09, 0x10};
  res = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);
  assert(res == 0);

  /* Serialize the message into a buffer */
  size_t bufSize = 512;
  uint8_t* serBuf = malloc(bufSize);
  assert(serBuf != NULL);
  size_t serSize = serializePlcMessage_t(msg, serBuf);
  assert(serSize > 0);

  assert(serBuf[0]  == 0x00);
  assert(serBuf[1]  == 0x00);
  assert(serBuf[2]  == 0x00);
  assert(serBuf[3]  == 0x01);
  assert(serBuf[4]  == 0x00);

  assert(serBuf[5]  == 0x16);
  assert(serBuf[6]  == 0x00);

  assert(serBuf[7]  == 0xF1);

  assert(serBuf[8]  == 0x28);
  assert(serBuf[9]  == 0x10);

  assert(serBuf[10] == 0x0E);
  assert(serBuf[11] == 0x10);

  assert(serBuf[12] == 0x09);
  assert(serBuf[13] == 0x10);

  assert(serBuf[14] == 0x37);

  assert(serBuf[15] == 0x06);
  assert(serBuf[16] == 0x68);
  assert(serBuf[17] == 0x07);

  assert(serBuf[18] == 0x27);
  assert(serBuf[19] == 0x00);

  assert(serBuf[20] == 0x03);
  assert(serBuf[21] == 0x00);

  assert(serBuf[22] == 0x28);
  assert(serBuf[23] == 0x00);

  assert(serBuf[24] == 0xFF);
  assert(serBuf[25] == 0xFF);

  assert(serBuf[26] == 0x1F);
  assert(serBuf[27] == 0x00);

  free(serBuf);
  freeMessage(msg);
  verbose("[Plc Facade] configWritePlcMessage (TOGGLE_SWITCH) ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Plc Facade ... \n\n");

  test_configWritePlcMessage_toggleSwitch();

  verbose("\n[Unit Testing] Plc Facade ... Done \n");
  return 0;
}

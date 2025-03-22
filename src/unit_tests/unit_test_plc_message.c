#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "plc/plc_message.h"
#include "common/verbose.h"
#include "common/flags.h"

void test_createPlcMessage() {
  verbose("[Plc Message] createPlcMessage ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  freeMessage(msg);
  verbose("[Plc Message] createPlcMessage ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_setAPDU_valid() {
  verbose("[Plc Message] setAPDU valid ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  uint8_t data[] = {0x55, 0x66};
  int ret = setAPDU(msg, APDU_WRITE_REQ, data, sizeof(data));
  assert(ret == 0);

  freeMessage(msg);
  verbose("[Plc Message] setAPDU valid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_setAPDU_invalid() {
  verbose("[Plc Message] setAPDU invalid ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  uint8_t data[] = {0x11, 0x22};
  int ret = setAPDU(msg, APDU_WRITE_RESP, data, sizeof(data));
  assert(ret != 0);

  freeMessage(msg);
  verbose("[Plc Message] setAPDU invalid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_setNPDU_valid() {
  verbose("[Plc Message] setNPDU valid ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  XwayAddr sender = createXwayAddr(1, 2, 3);
  XwayAddr receiver = createXwayAddr(4, 5, 6);
  uint8_t extAddr[2] = {0xDE, 0xAD};

  int ret = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);
  assert(ret == 0);

  freeMessage(msg);
  verbose("[Plc Message] setNPDU valid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_createXwayAddr() {
  verbose("[Plc Message] createXwayAddr ... \n");
  XwayAddr addr = createXwayAddr(0x28, 1 , 0);

  XwayAddr expected = ( 0x28 << 8 ) + (1 << 4) + (0 & 0x0F);
  assert(addr == expected);

  verbose("[Plc Message] createXwayAddr ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_serializePlcMessage_t() {
  verbose("[Plc Message] serializePlcMessage_t ... \n");

  uint8_t response[24];
  response[0] = 0x00;
  response[1] = 0x00;
  response[2] = 0x00;
  response[3] = 0x01;
  response[4] = 0x00;

  response[5] = 0x12;

  response[6] = 0x00;

  response[7] = 0xF1;

  response[8] = 0x0E;
  response[9] = 0x10;

  response[10] = 0x28;
  response[11] = 0x10;

  response[12] = 0x09;
  response[13] = 0x34;

  response[14] = 0x37;
  response[15] = 0x07;

  response[16] = 0x68;

  response[17] = 0x07;

  response[18] = 0x09;
  response[19] = 0x00;

  response[20] = 0x01;
  response[21] = 0x00;

  response[22] = 0x1F;
  response[23] = 0x00;

  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  /* Setup a dummy APDU */
  uint8_t data[] = {PC_CATEGORY, INTERNAL_DATA_SPACE, UNITE_TYPE_WORD, 0x09, 0x00, 0x01, 0x00, 0x1F, 0x00};
  int ret = setAPDU(msg, APDU_WRITE_REQ, data, sizeof(data) );
  assert(ret == 0);

  /* Setup NPDU */
  XwayAddr receiver = createXwayAddr(0x28, 1 , 0);
  XwayAddr sender = createXwayAddr( 0x0E, 1, 0);

  uint8_t extAddr[2] = {0x09, 0x34};
  ret = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);

  assert(ret == 0);

  size_t bufSize = 512*sizeof(uint8_t);
  uint8_t* serBuf = malloc(bufSize);
  assert(serBuf != NULL);

  size_t serSize = serializePlcMessage_t(msg, serBuf);
  assert(serSize > 0);

  assert(memcmp(serBuf, response, serSize) == 0);

  free(serBuf);
  freeMessage(msg);
  verbose("[Plc Message] serializePlcMessage_t ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_deserializePlcMessage_t_valid() {
  verbose("[Plc Message] deserializePlcMessage_t valid ... \n");

  /* Create and setup an original message */
  PlcMessage_t* origMsg = createPlcMessage();
  assert(origMsg != NULL);

  uint8_t apduData[] = {PC_CATEGORY, INTERNAL_DATA_SPACE, UNITE_TYPE_WORD, 0x09, 0x00, 0x01, 0x00, 0x1F, 0x00};
  int ret = setAPDU(origMsg, APDU_WRITE_REQ, apduData, sizeof(apduData));
  assert(ret == 0);

  XwayAddr sender = createXwayAddr(1, 2, 3);
  XwayAddr receiver = createXwayAddr(4, 5, 6);
  uint8_t extAddr[2] = {0xDE, 0xAD};
  ret = setNPDU(origMsg, NPDU_5WAY, sender, receiver, extAddr);
  assert(ret == 0);

  /* Serialize the original message */
  size_t bufSize = 512*sizeof(uint8_t);
  uint8_t* serBufOrig = malloc(bufSize);
  assert(serBufOrig != NULL);
  size_t serSizeOrig = serializePlcMessage_t(origMsg, serBufOrig);
  assert(serSizeOrig > 0);

  /* Now deserialize the message from the serialized buffer */
  PlcMessage_t* deserializedMsg = deserializePlcMessage_t(serBufOrig);
  assert(deserializedMsg != NULL);

  /* Re-serialize the deserialized message to compare with the original serialization */
  uint8_t* serBufDeser = malloc(bufSize);
  assert(serBufDeser != NULL);
  size_t serSizeDeser = serializePlcMessage_t(deserializedMsg, serBufDeser);
  assert(serSizeDeser > 0);

  /* Compare the sizes and contents */
  assert(memcmp(serBufOrig, serBufDeser, serSizeOrig) == 0);

  free(serBufOrig);
  free(serBufDeser);
  freeMessage(origMsg);
  freeMessage(deserializedMsg);
  verbose("[Plc Message] deserializePlcMessage_t valid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_ackMessage(){
  verbose("[Plc Message] ack message ... \n");

  uint8_t response[15];
  response[0] = 0x00;
  response[1] = 0x00;
  response[2] = 0x00;
  response[3] = 0x01;
  response[4] = 0x00;

  response[5] = 0x09;

  response[6] = 0x00;

  response[7] = 0xF1;

  response[8] = 0x28;
  response[9] = 0x10;

  response[10] = 0x0E;
  response[11] = 0x10;

  response[12] = 0x09;
  response[13] = 0x34;

  response[14] = 0xFE;

  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  /* Setup a dummy APDU */
  uint8_t data[] = {PC_CATEGORY, INTERNAL_DATA_SPACE, UNITE_TYPE_WORD, 0x09, 0x00, 0x01, 0x00, 0x1F, 0x00};
  int ret = setAPDU(msg, APDU_WRITE_REQ, data, sizeof(data) );
  assert(ret == 0);

  /* Setup NPDU */
  XwayAddr sender = createXwayAddr( 0x0E, 1, 0);
  XwayAddr receiver = createXwayAddr(0x28, 1 , 0);

  uint8_t extAddr[2] = {0x09, 0x34};
  ret = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);
  assert(ret == 0);

  PlcMessage_t* ack = createACK(msg, true);

  size_t bufSize = 512*sizeof(uint8_t);
  uint8_t* serBuf = malloc(bufSize);
  assert(serBuf != NULL);

  size_t serSize = serializePlcMessage_t(ack, serBuf);
  assert(serSize > 0);

  assert(memcmp(serBuf, response, serSize) == 0);

  free(serBuf);
  freeMessage(msg);
  freeMessage(ack);
  verbose("[Plc Message] ack message... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_freeMessage() {
  verbose("[Plc Message] freeMessage ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  freeMessage(msg);
  verbose("[Plc Message] freeMessage ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Plc Message ... \n\n");

  test_createPlcMessage();
  test_setAPDU_valid();
  test_setAPDU_invalid();
  test_setNPDU_valid();
  test_createXwayAddr();
  test_serializePlcMessage_t();
  test_deserializePlcMessage_t_valid();
  test_ackMessage();
  test_freeMessage();

  verbose("\n[Unit Testing] Plc Message ... Done \n");
  return 0;
}

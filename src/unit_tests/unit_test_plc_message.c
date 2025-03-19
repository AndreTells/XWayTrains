#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "plc/plc_message.h"
#include "common/verbose.h"
#include "common/flags.h"

/*
 * Test createPlcMessage()
 */
void test_createPlcMessage() {
  verbose("[Plc Message] createPlcMessage ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  freeMessage(msg);
  verbose("[Plc Message] createPlcMessage ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test setAPDU with a valid input.
 * For APDU_WRITE_REQ we assume that data is allowed.
 */
void test_setAPDU_valid() {
  verbose("[Plc Message] setAPDU valid ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  /* Provide dummy data; the API does not take a data length so the implementation
     must know how to handle the data (for example, by using a fixed size or a terminator). */
  uint8_t data[] = {0x55, 0x66};
  int ret = setAPDU(msg, APDU_WRITE_REQ, data, 2);
  assert(ret == 0);

  freeMessage(msg);
  verbose("[Plc Message] setAPDU valid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test setAPDU with an invalid case.
 * We assume that for APDU_WRITE_RESP the function returns an error if data is provided.
 */
void test_setAPDU_invalid() {
  verbose("[Plc Message] setAPDU invalid ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  uint8_t data[] = {0x11, 0x22};
  int ret = setAPDU(msg, APDU_WRITE_RESP, data, 2);
  assert(ret != 0);

  freeMessage(msg);
  verbose("[Plc Message] setAPDU invalid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test setNPDU with valid parameters.
 * Since the internal structure is hidden, we simply verify that the API returns success.
 */
void test_setNPDU_valid() {
  verbose("[Plc Message] setNPDU valid ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  /* Create sender and receiver addresses using the provided API */
  XwayAddr sender = createXwayAddr(1, 2, 3);
  XwayAddr receiver = createXwayAddr(4, 5, 6);
  uint8_t extAddr[2] = {0xDE, 0xAD};

  int ret = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);
  assert(ret == 0);

  freeMessage(msg);
  verbose("[Plc Message] setNPDU valid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test createXwayAddr.
 * Here we compare the result to an expected value based on the function's algorithm.
 */
void test_createXwayAddr() {
  verbose("[Plc Message] createXwayAddr ... \n");
  XwayAddr addr = createXwayAddr(10, 20, 5);

  /* Expected: ( (int16_t)10 << 8 ) + (20 << 4) + (5 & 0x0F) */
  int16_t expandedStation = 10;
  XwayAddr expected = (expandedStation << 8) + (20 << 4) + (5 & 0x0F);
  assert(addr == expected);

  verbose("[Plc Message] createXwayAddr ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test serializePlcMessage_t.
 * This test creates a message, sets an APDU and NPDU, serializes the message,
 * and verifies that a nonzero size is returned.
 */
void test_serializePlcMessage_t() {
  verbose("[Plc Message] serializePlcMessage_t ... \n");
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);

  /* Set up an APDU */
  uint8_t data[] = {0x55, 0x66};
  int ret = setAPDU(msg, APDU_WRITE_REQ, data, 2);
  assert(ret == 0);

  /* Set up NPDU (if applicable) */
  XwayAddr sender = createXwayAddr(1, 2, 3);
  XwayAddr receiver = createXwayAddr(4, 5, 6);
  uint8_t extAddr[2] = {0xDE, 0xAD};
  ret = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);
  assert(ret == 0);

  /* Serialize into a buffer and check the returned size */
  size_t bufSize = 512;
  uint8_t* serBuf = malloc(bufSize);
  assert(serBuf != NULL);

  size_t serSize = serializePlcMessage_t(msg, serBuf);
  assert(serSize > 0);

  free(serBuf);
  freeMessage(msg);
  verbose("[Plc Message] serializePlcMessage_t ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test deserializePlcMessage_t.
 * Since the function is not implemented, we expect a NULL return.
 */
void test_deserializePlcMessage_t() {
  verbose("[Plc Message] deserializePlcMessage_t ... \n");
  uint8_t dummyBuf[10] = {0};
  PlcMessage_t* msg = deserializePlcMessage_t(dummyBuf, sizeof(dummyBuf));
  assert(msg == NULL);
  verbose("[Plc Message] deserializePlcMessage_t ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

/*
 * Test freeMessage.
 * This simply creates and then frees a message.
 */
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
  test_deserializePlcMessage_t();
  test_freeMessage();

  verbose("\n[Unit Testing] Plc Message ... Done \n");
  return 0;
}

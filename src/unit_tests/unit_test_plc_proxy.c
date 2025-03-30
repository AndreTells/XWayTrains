#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "plc/plc_proxy.h"
#include "plc/plc_message.h"
#include "plc/plc_facade.h"
#include "plc/model_info.h"
#include "common/comm_general.h"
#include "common/time_out.h"
#include "common/verbose.h"
#include "common/flags.h"

#define HOST_ADDR "127.0.0.1"
#define SERVER_ADDR "10.31.125.14"

const uint8_t plcStation = 0x0E;

PlcMessage_t* unitTestTryGetPlcMessage(int fd){
  uint8_t serMsg[MAX_MSG_SIZE];

  int resWait = fileDescriptorTimedWait(fd);
  if (resWait < 0) {
    return NULL;
  }

  // Read enough to get the message size
  ssize_t bytesRead = 0;
  while (bytesRead < MSG_SIZE_POS) {
    ssize_t res = read(fd, serMsg + bytesRead, MSG_SIZE_POS - bytesRead);
    if (res == -1) {
      perror("read error");
      return NULL;
    } else if (res == 0) {
      // Connection closed
      return NULL;
    }
    bytesRead += res;
  }

  uint8_t msgSize = serMsg[MSG_SIZE_POS - 1];

  // Validate msgSize to prevent buffer overflow
  if (msgSize > MAX_MSG_SIZE - MSG_SIZE_POS) {
    fprintf(stderr, "Invalid message size: %u\n", msgSize);
    return NULL;
  }

  // Read the rest of the message
  bytesRead = 0;
  while (bytesRead < msgSize) {
    ssize_t res = read(fd, serMsg + MSG_SIZE_POS + bytesRead, msgSize - bytesRead);
    if (res == -1) {
      perror("read error");
      return NULL;
    } else if (res == 0) {
      return NULL;
    }
    bytesRead += res;
  }

  PlcMessage_t* msg = deserializePlcMessage(serMsg);

  return msg;
}

void test_initPlcProxy_invalid() {
  verbose("[Plc Proxy] initPlcProxy Invalid ... \n");
  PlcProxy_t* proxy = initPlcProxy(NULL, 0, 0);
  assert(proxy == NULL);
  verbose("[Plc Proxy] initPlcProxy Invalid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_init_endPlcProxy() {
  verbose("[Plc Proxy] init & endPlcProxy ... \n");
  uint8_t plc_station = 0x0E;
  uint8_t plc_port = 20;
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, plc_port, plc_station);

  assert(proxy != NULL);

  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] init & endPlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_sendMessagePlcProxy() {
  uint8_t plc_port = 30;
  verbose("[Plc Proxy] sendMessagePlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, plc_port, plcStation);

  assert(proxy != NULL);

  /* making a message */
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  int res;

  uint8_t pc_station = 0x28;
  res = configWritePlcMessage(msg, TOGGLE_SWITCH, pc_station , TRAIN_1, SWITCH_GROUP_31);
  assert(res == 0);
  // attempting to send the message

  ssize_t ret = sendMessagePlcProxy(proxy, msg, pc_station);
  assert(ret > 0);

  freeMessage(msg);
  ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] sendMessagePlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_readMessagePlcProxy() {
  uint8_t plc_port = 31;
  verbose("[Plc Proxy] readMessagePlcProxy ... \n");

  int serverFd = tcpCreateSocketWrapper(true, "", plc_port);
  assert(serverFd > 0);

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
  response[13] = 0x34;  // it answers a port_number

  response[14] = 0x37;

  //
  response[15] = 0x07;
  response[16] = 0x68;
  response[17] = 0x07;

  response[18] = 0x09; //
  response[19] = 0x00;

  response[20] = 0x01;
  response[21] = 0x00;
  response[22] = 0x1F;
  response[23] = 0x00;

  (void)write(serverFd, &response, 24);

  uint8_t plc_station = 0x0E;
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, plc_port, plc_station);

  assert(proxy != NULL);

  PlcMessage_t* receivedMsg = readMessagePlcProxy(proxy, TRAIN_1);
  assert(receivedMsg != NULL);
  free(receivedMsg);

  // checking if ack was sent
  PlcMessage_t* ack = unitTestTryGetPlcMessage(serverFd);
  assert( ack != NULL);
  assert( compareMsgType(ack, APDU_WRITE_RESP));
  free(ack);


  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] readMessagePlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_readMultipleMessagePlcProxy() {
  uint8_t plc_port = 32;
  verbose("[Plc Proxy] readMultipleMessagePlcProxy ... \n");

  int serverFd = tcpCreateSocketWrapper(true, "", plc_port);
  assert(serverFd > 0);

  // desired message
  uint8_t response1[24];
  response1[0] = 0x00;
  response1[1] = 0x00;
  response1[2] = 0x00;
  response1[3] = 0x01;
  response1[4] = 0x00;

  response1[5] = 0x12;

  response1[6] = 0x00;

  response1[7] = 0xF1;

  response1[8] = 0x0E;
  response1[9] = 0x10;

  response1[10] = 0x28;
  response1[11] = 0x10;

  response1[12] = 0x09;
  response1[13] = 0x34;  // it answers a port_number

  response1[14] = 0x37;

  //
  response1[15] = 0x07;
  response1[16] = 0x68;
  response1[17] = 0x07;

  response1[18] = 0x09; //
  response1[19] = 0x00;

  response1[20] = 0x01;
  response1[21] = 0x00;
  response1[22] = 0x1F;
  response1[23] = 0x00;

  uint8_t response2[15];
  response2[0] = 0x00;
  response2[1] = 0x00;
  response2[2] = 0x00;
  response2[3] = 0x01;
  response2[4] = 0x00;

  response2[5] = 0x09;

  response2[6] = 0x00;

  response2[7] = 0xF1;

  response2[10] = 0x0E;
  response2[11] = 0x10;

  response2[8] = 0x29;
  response2[9] = 0x10;

  response2[12] = 0x19;
  response2[13] = 0x34;

  response2[14] = 0xFE;

  (void)write(serverFd, &response2, 15);
  (void)write(serverFd, &response1, 24);

  uint8_t plc_station = 0x0E;
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, plc_port, plc_station);

  assert(proxy != NULL);

  PlcMessage_t* receivedMsg = readMessagePlcProxy(proxy, TRAIN_1);
  assert(receivedMsg != NULL);
  free(receivedMsg);

  // checking if ack was sent
  PlcMessage_t* ack = unitTestTryGetPlcMessage(serverFd);
  assert( ack != NULL);
  assert(compareMsgType(ack, APDU_WRITE_RESP));
  free(ack);


  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] readMultipleMessagePlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Plc Proxy ... \n\n");

  test_initPlcProxy_invalid();
  test_init_endPlcProxy();
  test_sendMessagePlcProxy();
  test_readMessagePlcProxy();
  test_readMultipleMessagePlcProxy();

  verbose("\n[Unit Testing] Plc Proxy ... Done \n");
  return 0;
}

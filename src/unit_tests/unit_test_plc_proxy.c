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
#define PLC_PORT 502

const uint8_t plcStation = 0x0E;

PlcMessage_t* unitTestTryGetPlcMessage(int fd){
  uint8_t serMsg[MAX_MSG_SIZE];

  int resWait = fileDescriptorTimedWait(fd);
  if(resWait < 0){
    return NULL;
  }

  ssize_t resSize = read(fd, serMsg, MAX_MSG_SIZE);
  if (resSize == 0) {
    return NULL;
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
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, PLC_PORT, plc_station);

  assert(proxy != NULL);

  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] init & endPlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_sendMessagePlcProxy() {
  verbose("[Plc Proxy] sendMessagePlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, PLC_PORT, plcStation);

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
  verbose("[Plc Proxy] readMessagePlcProxy ... \n");

  int serverFd = tcpCreateSocketWrapper(true, "", PLC_PORT);
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
  PlcProxy_t* proxy = initPlcProxy(SERVER_ADDR, PLC_PORT, plc_station);

  assert(proxy != NULL);

  PlcMessage_t* receivedMsg = readMessagePlcProxy(proxy, TRAIN_1);
  assert(receivedMsg != NULL);
  free(receivedMsg);

  // checking if ack was sent
  PlcMessage_t* ack = unitTestTryGetPlcMessage(serverFd);
  assert( ack != NULL);
  assert( compareMsgType(ack, APDU_WRITE_RESP) == 0);
  free(ack);


  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] readMessagePlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Plc Proxy ... \n\n");

  test_initPlcProxy_invalid();
  test_init_endPlcProxy();
  test_sendMessagePlcProxy();
  test_readMessagePlcProxy();

  verbose("\n[Unit Testing] Plc Proxy ... Done \n");
  return 0;
}

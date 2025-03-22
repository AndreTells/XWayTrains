#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "plc/plc_proxy.h"
#include "plc/plc_message.h"
#include "plc/plc_facade.h"
#include "plc/model_info.h"
#include "common/verbose.h"
#include "common/flags.h"

#define HOST_ADDR "127.0.0.1"
#define SERVER_ADDR "10.31.125.14"
#define PLC_PORT 502

void test_initPlcProxy_invalid() {
  verbose("[Plc Proxy] initPlcProxy Invalid ... \n");
  PlcProxy_t* proxy = initPlcProxy(NULL, NULL, -1);
  assert(proxy == NULL);
  verbose("[Plc Proxy] initPlcProxy Invalid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_init_endPlcProxy() {
  verbose("[Plc Proxy] init & endPlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT);
  assert(proxy != NULL);

  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] init & endPlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_sendMessagePlcProxy() {
  verbose("[Plc Proxy] sendMessagePlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT);
  assert(proxy != NULL);

  /* making a message */
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

  // attempting to send the message

  int ret = sendMessagePlcProxy(proxy, msg);
  assert(ret == 0);

  freeMessage(msg);
  ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] sendMessagePlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_readMessagePlcProxy() {
  verbose("[Plc Proxy] readMessagePlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT);
  assert(proxy != NULL);

  PlcMessage_t* receivedMsg = readMessagePlcProxy(proxy, 0);
  assert(receivedMsg == NULL);
  free(receivedMsg);

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

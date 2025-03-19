#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "plc/plc_proxy.h"
#include "plc/plc_message.h"
#include "common/verbose.h"
#include "common/flags.h"

void test_initPlcProxy_invalid() {
  verbose("[Plc Proxy] initPlcProxy Invalid ... \n");
  PlcProxy_t* proxy = initPlcProxy(NULL);
  assert(proxy == NULL);
  verbose("[Plc Proxy] initPlcProxy Invalid ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_init_endPlcProxy() {
  verbose("[Plc Proxy] init & endPlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy("127.0.0.1");
  assert(proxy != NULL);

  int ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] init & endPlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_sendMessagePlcProxy() {
  verbose("[Plc Proxy] sendMessagePlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy("127.0.0.1");
  assert(proxy != NULL);

  /* Assume getNullMessage() returns a dummy PlcMessage_t pointer
     and that its allocated size is used by getMessageSize(). */
  PlcMessage_t* dummyMsg = getNullMessage();
  assert(dummyMsg != NULL);

  int ret = sendMessagePlcProxy(proxy, dummyMsg);
  assert(ret == 0);

  free(dummyMsg);  // free if getNullMessage() allocates memory
  ret = endPlcProxy(proxy);
  assert(ret == 0);
  verbose("[Plc Proxy] sendMessagePlcProxy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_readMessagePlcProxy() {
  verbose("[Plc Proxy] readMessagePlcProxy ... \n");
  PlcProxy_t* proxy = initPlcProxy("127.0.0.1");
  assert(proxy != NULL);

  PlcMessage_t* dummyMsg = getNullMessage();
  assert(dummyMsg != NULL);

  int ret = sendMessagePlcProxy(proxy, dummyMsg);
  assert(ret == 0);
  free(dummyMsg);  // free the dummy message

  /* Now, attempt to read the message via the proxy */
  PlcMessage_t* receivedMsg = readMessagePlcProxy(proxy, 0);
  assert(receivedMsg != NULL);
  free(receivedMsg);

  ret = endPlcProxy(proxy);
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "train_manager/resource_manager_proxy.h"
#include "common/flags.h"
#include "common/verbose.h"


int resource_manager_proxy_test(void);

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);
  verbose("[Unit Testing] Resource Manager Proxy ... \n\n");

  resource_manager_proxy_test();

  verbose("\n[Unit Testing] Resource Manager Proxy ... Done \n");
  return 0;
}

int resource_manager_proxy_test(void) {
  verbose("[Resource Manager Proxy] Init with NULL ... \n");
  assert(initResourceManagerProxy(NULL) == NULL);
  verbose("[Resource Manager Proxy] Init with NULL ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager Proxy] Init Success ... \n");
  ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1");
  assert(proxy != NULL);
  verbose("[Resource Manager Proxy] Init Success ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager Proxy] End with NULL ... \n");
  assert(endResourceManagerProxy(NULL) == -1);
  verbose("[Resource Manager Proxy] End with NULL ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager Proxy] Request Resource Invalid Client ... \n");
  assert(requestResource(proxy, 42, 87) == -1);
  verbose("[Resource Manager Proxy] Request Resource Invalid Client ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager Proxy] Release Resource Invalid Client ... \n");
  assert(releaseResource(proxy, 42, 32) == -1);
  verbose("[Resource Manager Proxy] Release Resource Invalid Client ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager Proxy] Request Resource Success ... \n");
  assert(requestResource(proxy, 42, 0) == 0);
  verbose("[Resource Manager Proxy] Request Resource Success ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager Proxy] Release Resource Success ... \n");
  assert(releaseResource(proxy, 42, 0) == 0);
  verbose("[Resource Manager Proxy] Release Resource Success ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  endResourceManagerProxy(proxy);
  return 0;
}

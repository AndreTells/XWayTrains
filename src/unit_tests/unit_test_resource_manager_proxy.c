#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "train_manager/resource_manager_proxy.h"
#include "common/resource_request.h"
#include "common/flags.h"
#include "common/verbose.h"
#include "common/comm_general.h"


int resource_manager_proxy_init_invalid(void);
int resource_manager_proxy_init_end(void);
int resource_manager_proxy_request_release(void);

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);
  verbose("[Unit Testing] Resource Manager Proxy ... \n\n");

  resource_manager_proxy_init_invalid();
  resource_manager_proxy_init_end();
  resource_manager_proxy_request_release();

  verbose("\n[Unit Testing] Resource Manager Proxy ... Done \n");
  return 0;
}

int resource_manager_proxy_init_invalid(void) {
  verbose("[Resource Manager Proxy] Init with NULL ... \n");
  assert(initResourceManagerProxy(NULL, 0) == NULL);
  verbose("[Resource Manager Proxy] Init with NULL ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return 0;

}

int resource_manager_proxy_init_end(void){

  verbose("[Resource Manager Proxy] Init End ... \n");
  ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1",40);
  assert(proxy != NULL);

  assert(endResourceManagerProxy(proxy) == 0);
  verbose("[Resource Manager Proxy] Init End ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  return 0;
}

int resource_manager_proxy_request_release(void){
  verbose("[Resource Manager Proxy] Request release ... \n");
  ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1",40);
  assert(proxy != NULL);

  assert(requestResource(proxy,LOCK_RESOURCE, 42, 0) == 0);

  assert(requestResource(proxy,RELEASE_RESOURCE, 42, 0) == 0);


  assert(endResourceManagerProxy(proxy) == 0);
  verbose("[Resource Manager Proxy] Request release ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return 0;
}

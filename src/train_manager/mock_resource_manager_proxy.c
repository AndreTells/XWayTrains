#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/resource_request.h"
#include "common/verbose.h"
#include "plc/model_info.h"
#include "train_manager/resource_manager_proxy.h"


struct ResourceManagerProxy_t {
  int test;
};


ResourceManagerProxy_t* initResourceManagerProxy(char* resManagerIpAddr) {

  ResourceManagerProxy_t* resManager =
      (ResourceManagerProxy_t*)malloc(sizeof(ResourceManagerProxy_t));

  return resManager;
}

int endResourceManagerProxy(ResourceManagerProxy_t* resManager) {
  free(resManager);
  return 0;
}

int requestResource(ResourceManagerProxy_t* resManager,
                    ResourceRequestType_e reqType, int resourceId,
                    int clientId){
  return 0;
}

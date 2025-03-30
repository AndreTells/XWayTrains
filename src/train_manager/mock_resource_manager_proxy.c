#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/resource_request.h"
#include "plc/model_info.h"
#include "train_manager/resource_manager_proxy.h"

struct ResourceManagerProxy_t {
  int test;
};

ResourceManagerProxy_t* initResourceManagerProxy(
    [[maybe_unused]] char* resManagerIpAddr,
    [[maybe_unused]] const uint16_t port) {
  ResourceManagerProxy_t* resManager =
      (ResourceManagerProxy_t*)malloc(sizeof(ResourceManagerProxy_t));

  return resManager;
}

int endResourceManagerProxy(ResourceManagerProxy_t* resManager) {
  free(resManager);
  return 0;
}

int requestResource([[maybe_unused]] ResourceManagerProxy_t* resManager,
                    [[maybe_unused]] ResourceRequestType_e reqType,
                    [[maybe_unused]] uint8_t* resourceList,
                    [[maybe_unused]] uint8_t resourceListSize,
                    [[maybe_unused]] const enum TrainId_e clientId) {
  return 0;
}

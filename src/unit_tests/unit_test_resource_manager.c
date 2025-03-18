#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "resource_manager/resource_manager.h"
#include "resource_manager/request_queue.h"
#include "common/resource_request.h"
#include "common/verbose.h"
#include "common/flags.h"

int resource_manager_test(void);

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);
  verbose("[Unit Testing] Resource Manager ... \n\n");

  resource_manager_test();

  verbose("\n[Unit Testing] Resource Manager ... Done \n");
  return 0;
}

int resource_manager_test(void) {
  verbose("[Resource Manager] Init ... \n");
  ResourceManager_t* manager = initResourceManager(NULL, "127.0.0.1", 8080);
  assert(manager != NULL);
  verbose("[Resource Manager] Init ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager] Accept Train Manager ... \n");
  assert(acceptTrainManager(manager) == 0);
  verbose("[Resource Manager] Accept Train Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[Resource Manager] Cleanup ... \n");
  endResourceManager(manager);
  verbose("[Resource Manager] Cleanup ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  return 0;
}

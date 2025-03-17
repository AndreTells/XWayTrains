#include "resource_manager/resource_database_proxy.h"
#include "resource_manager/resource_manager.h"
#include "common/verbose.h"
#include <unistd.h>
#include <stdbool.h>

int main() {
  // TODO: get actual IP
  setVerbose(true);

  verbose("[RESOURCE_MANAGER_MAIN]: Database ... \n");
  ResourceDataBaseProxy_t* safeDatabase = initResourceDatabaseProxy();
  if(safeDatabase == NULL){
    verbose("[RESOURCE_MANAGER_MAIN]: Database ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  verbose("[RESOURCE_MANAGER_MAIN]: Database ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[RESOURCE_MANAGER_MAIN]: Initializing Resource Manager ... \n");
  ResourceManager_t* manager = initResourceManager(safeDatabase, "127.0.0.1", 8080);
  if(manager == NULL){
    verbose("[RESOURCE_MANAGER_MAIN]: Initializing Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  verbose("[RESOURCE_MANAGER_MAIN]: Initializing Resource Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  // TODO: add a way so the program can actually terminate
  while (true) {
    verbose("[RESOURCE_MANAGER_MAIN]: Waiting for a Train Manager Connection ... \n");
    int res = acceptTrainManager(manager);
    if(res < 0){
      verbose("[RESOURCE_MANAGER_MAIN]: Waiting for a Train Manager Connection ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
      return -1;
    }
    verbose("[RESOURCE_MANAGER_MAIN]: Waiting for a Train Manager Connection ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  }

  (void)endResourceManager(manager);
  (void)endResourceDataBaseProxy(safeDatabase);
  return 0;
}

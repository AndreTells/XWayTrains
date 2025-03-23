#include "resource_manager/resource_database_proxy.h"
#include "resource_manager/resource_manager.h"
#include "common/verbose.h"
#include "common/flags.h"

#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

ResourceDataBaseProxy_t* safeDatabase = NULL;
ResourceManager_t* manager = NULL;
void handle_sigint(int sig) {
  if(sig != SIGINT){
    return;
  }
  verbose("\n[RESOURCE_MANAGER_MAIN]: Ctr-C Captured. Exiting Program \n");
  if(manager != NULL){
    (void)endResourceManager(manager);
  }
  if(safeDatabase != NULL){
    (void)endResourceDataBaseProxy(safeDatabase);
  }
  exit(0);
}


int main(int argc, char *argv[]) {
  // setting up signal handling
  signal(SIGINT, handle_sigint);

  // checking command line arguments
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  char* ip_address;
  bool ip_received = get_flag_value(argc,argv,IP_ADDRESS_FLAG,&ip_address);
  if(!ip_received){
    verbose("[RESOURCE_MANAGER_MAIN]: ERROR No Ip Address Received\n");
    return -1;
  }

  verbose("[RESOURCE_MANAGER_MAIN]: Utilize Ctr-C to end this program\n");

  verbose("[RESOURCE_MANAGER_MAIN]: Database ... \n");
  safeDatabase = initResourceDatabaseProxy();

  if(safeDatabase == NULL){
    verbose("[RESOURCE_MANAGER_MAIN]: Database ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  verbose("[RESOURCE_MANAGER_MAIN]: Database ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[RESOURCE_MANAGER_MAIN]: Initializing Resource Manager ... \n");
  manager = initResourceManager(safeDatabase, ip_address, 8080);

  if(manager == NULL){
    verbose("[RESOURCE_MANAGER_MAIN]: Initializing Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  verbose("[RESOURCE_MANAGER_MAIN]: Initializing Resource Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  while (true) {
    verbose("[RESOURCE_MANAGER_MAIN]: Waiting for a Train Manager Connection ... \n");
    int res = acceptTrainManager(manager);
    if(res < 0){
      verbose("[RESOURCE_MANAGER_MAIN]: Waiting for a Train Manager Connection ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
      return -1;
    }
    verbose("[RESOURCE_MANAGER_MAIN]: Waiting for a Train Manager Connection ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  }
}

/*
 * BUG: in the train 4's route the train does not invert at the end, maybe
 * consider adding a pause command to the interpreter to wait for the train
 * stop before attempting to invert
 */
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include "plc/plc_proxy.h"
#include "train_manager/train.h"
#include "train_manager/interpreter.h"
#include "train_manager/resource_manager_proxy.h"
#include "common/flags.h"
#include "common/verbose.h"

#define HOST_IP "172.31.71.25"

#define RES_MANAGER_REMOTE_IP "127.0.0.1"
#define RES_MANAGER_PORT 8080

#define PLC_REMOTE_IP "10.31.125.14"
#define PLC_PORT 502
#define XWAY_HOST_STATION 0x28
#define XWAY_REMOTE_STATION 0x0E
#define XWAY_NETWORK 1
#define XWAY_PORT 0

PlcProxy_t* plc = NULL;

ResourceManagerProxy_t* resManager = NULL;

void handle_sigint(int sig) {
  if (sig != SIGINT) {
    return;
  }
  verbose(
      "\n[INTERPRETER TEST]: Ctr-C Captured. Exiting Program \n");

  if (resManager != NULL) {
    (void)endResourceManagerProxy(resManager);
  }

  if(plc != NULL){
    (void)endPlcProxy(plc);
  }

  exit(0);
}

int main(int argc, char* argv[]){
  signal(SIGINT, handle_sigint);

  // checking for flags
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  char* routeFilePath;
  int hasFile = get_flag_value(argc,argv,"--route",&routeFilePath);
  if(!hasFile){
    verbose("[Train Test] no route specified\n");
    return -1;
  }

  verbose("[Train Test]: using route %s \n", routeFilePath);

  verbose("[Train Test] connecting to ressource manager\n");
  resManager = initResourceManagerProxy(RES_MANAGER_REMOTE_IP, RES_MANAGER_PORT);
  assert(resManager != NULL);

  verbose("[Train Test] connecting to plc\n");
  plc = initPlcProxy(HOST_IP, PLC_REMOTE_IP, PLC_PORT);
  int netRes = setXwayAddrs(plc ,  XWAY_HOST_STATION, XWAY_REMOTE_STATION,
                            XWAY_NETWORK, XWAY_PORT);

  assert(plc != NULL);
  assert(netRes == 0);

  Train_t* train = initTrain(plc, resManager, routeFilePath);
  assert(train != NULL);

  int execRes = executeRoute(train, XWAY_HOST_STATION);
  sleep(3);

  assert(execRes == 0);

  endTrain(train);
  endResourceManagerProxy(resManager);
  endPlcProxy(plc);
  return 0;
}

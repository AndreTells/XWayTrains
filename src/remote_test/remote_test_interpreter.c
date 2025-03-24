#include <assert.h>
#include "plc/plc_proxy.h"
#include "train_manager/train.h"
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

  // Initialising the resource manager
  verbose("[INTERPRETER TEST] connecting to ressource manager\n");
  // checking for flags
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  resManager = initResourceManagerProxy(RES_MANAGER_REMOTE_IP, RES_MANAGER_PORT);
  assert(resManager != NULL);

  verbose("[INTERPRETER TEST] connecting to plc\n");
  plc = initPlcProxy(HOST_IP, PLC_REMOTE_IP, PLC_PORT);

  uint8_t pc_station = 0x28;
  uint8_t plc_station = 0x0E;

  uint8_t network = 1;
  uint8_t port = 0;
  int netRes = setXwayAddrs(plc ,  XWAY_HOST_STATION, XWAY_REMOTE_STATION,
                            XWAY_NETWORK, XWAY_PORT);

  assert(plc != NULL);
  assert(netRes == 0);

  verbose("[INTERPRETER TEST] creating Train object\n");
  Train_t* train = initTrain(plc, resManager," ");


  verbose("[INTERPRETER TEST] setting the train Id\n");
  char cmd1[] = "trainId 1";
  assert(executeCommand(cmd1, train, plc, XWAY_HOST_STATION, resManager) == 0);

  verbose("[INTERPRETER TEST] setting rail 22\n");
  char cmd2[] = "plc rail 22";
  assert(executeCommand(cmd2, train, plc, XWAY_HOST_STATION, resManager) == 0);

  return 0;
}

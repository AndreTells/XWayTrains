#include <assert.h>
#include <signal.h>

#include "common/flags.h"
#include "common/verbose.h"
#include "plc/plc_proxy.h"
#include "train_manager/interpreter.h"
#include "train_manager/resource_manager_proxy.h"
#include "train_manager/train.h"

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
  verbose("\n[INTERPRETER TEST]: Ctr-C Captured. Exiting Program \n");

  if (resManager != NULL) {
    (void)endResourceManagerProxy(resManager);
  }

  if (plc != NULL) {
    (void)endPlcProxy(plc);
  }

  exit(0);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, handle_sigint);

  // Initialising the resource manager
  verbose("[INTERPRETER TEST] connecting to ressource manager\n");
  // checking for flags
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  resManager =
      initResourceManagerProxy(RES_MANAGER_REMOTE_IP, RES_MANAGER_PORT);
  assert(resManager != NULL);

  verbose("[INTERPRETER TEST] connecting to plc\n");
  plc = initPlcProxy(PLC_REMOTE_IP, PLC_PORT, XWAY_REMOTE_STATION);

  assert(plc != NULL);

  verbose("[INTERPRETER TEST] creating Train object\n");
  Train_t* train = initTrain(plc, resManager, " ", XWAY_HOST_STATION);

  verbose("[INTERPRETER TEST] setting the train Id\n");
  char cmd1[] = "trainId 1";
  assert(executeCommand(cmd1, train, plc, resManager) == 0);

  verbose("[INTERPRETER TEST] taking resource resource 0\n");
  char cmdr1[] = "resource lock 3";
  assert(executeCommand(cmdr1, train, plc,  resManager) == 0);

  verbose("[INTERPRETER TEST] setting switch 31\n");
  char cmds1[] = "plc switch 31";
  assert(executeCommand(cmds1, train, plc, resManager) == 0);

  verbose("[INTERPRETER TEST] setting switch 31\n");
  char cmds2[] = "plc switch 22";
  assert(executeCommand(cmds2, train, plc, resManager) == 0);

  verbose("[INTERPRETER TEST] setting switch 31\n");
  char cmds3[] = "plc switch 33";
  assert(executeCommand(cmds3, train, plc, resManager) == 0);

  verbose("[INTERPRETER TEST] setting switch 31\n");
  char cmds4[] = "plc switch 23";
  assert(executeCommand(cmds4, train, plc, resManager) == 0);

  for (int i = 0; i < 3; i++) {
    verbose("[INTERPRETER TEST] setting rail 3\n");
    char cmd2[] = "plc rail 3";
    assert(executeCommand(cmd2, train, plc, resManager) ==
           0);

    verbose("[INTERPRETER TEST] setting rail 23\n");
    char cmd3[] = "plc rail 23";
    assert(executeCommand(cmd3, train, plc, resManager) ==
           0);

    verbose("[INTERPRETER TEST] setting rail 10\n");
    char cmd4[] = "plc rail 10";
    assert(executeCommand(cmd4, train, plc, resManager) ==
           0);

    verbose("[INTERPRETER TEST] setting rail 29\n");
    char cmd5[] = "plc rail 29";
    assert(executeCommand(cmd5, train, plc, resManager) ==
           0);

    verbose("[INTERPRETER TEST] setting rail 19\n");
    char cmd6[] = "plc rail 19";
    assert(executeCommand(cmd6, train, plc, resManager) ==
           0);
  }

  endResourceManagerProxy(resManager);
  endPlcProxy(plc);
  return 0;
}

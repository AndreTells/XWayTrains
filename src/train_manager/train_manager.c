#include <stdint.h>
#include <assert.h>
#include <bits/pthreadtypes.h>
#include <signal.h>
#include <unistd.h>

#include "common/comm_general.h"
#include "common/flags.h"
#include "common/verbose.h"
#include "plc/model_info.h"
#include "train_manager/train.h"

#define HOST_IP "172.31.71.25"

#define RES_MANAGER_REMOTE_IP "127.0.0.1"
#define RES_MANAGER_PORT 8080

#define PLC_REMOTE_IP "10.31.125.14"
#define PLC_PORT 502
#define XWAY_HOST_STATION1 0x28
#define XWAY_HOST_STATION2 0x29
#define XWAY_REMOTE_STATION 0x0E
#define XWAY_NETWORK 1
#define XWAY_PORT 0

#define NUM_LAPS 3

PlcProxy_t* plc = NULL;

ResourceManagerProxy_t* resManager = NULL;

void handle_sigint(int sig) {
  if (sig != SIGINT) {
    return;
  }
  verbose("\n[TRAIN MANAGER]: Ctrl-C Captured. Exiting Program \n");

  if (resManager != NULL) {
    (void)endResourceManagerProxy(resManager);
  }

  if (plc != NULL) {
    (void)endPlcProxy(plc);
  }

  exit(0);
}

struct train_thread_attr{
  char * routeFilePath;
  uint8_t station;
};

/**
 * @brief Entry point for the train thread
 * @param[in] data Pointer to the Train_t instance
 * @return Thread exit status (always NULL)
 */
void * trainThread(struct train_thread_attr* attr) {
  verbose("[TrainThread][%s] Initializing ... \n", attr->routeFilePath);
  Train_t* train = initTrain(plc, resManager, attr->routeFilePath, attr->station);
  assert(train != NULL);

  for(int i=0;i<NUM_LAPS;i++){
    int execRes = executeRoute(train);

    assert(execRes == 0);
    verbose("\n\n\nlap completed\n\n\n");
  }

  endTrain(train);
  return NULL;
}

int main(const int argc, char** argv) {
  signal(SIGINT, handle_sigint);

  // checking for flags
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  char* routeFilePath1;
  if (!get_flag_value(argc, argv, "--route1", &routeFilePath1)) {
    verbose("[Train Manager] no route1 specified\n");
    exit(EXIT_FAILURE);
  }
  verbose("[Train Manager]: using route1 %s \n", routeFilePath1);

  char* routeFilePath2;
  if (!get_flag_value(argc, argv, "--route2", &routeFilePath2)) {
    verbose("[Train Manager] no route2 specified\n");
    exit(EXIT_FAILURE);
  }
  verbose("[Train Manager]: using route2 %s \n", routeFilePath2);

  verbose("[Train Manager] connecting to ressource manager\n");
  resManager =
      initResourceManagerProxy(RES_MANAGER_REMOTE_IP, RES_MANAGER_PORT);
  assert(resManager != NULL);

  verbose("[Train Manager] connecting to plc\n");
  plc = initPlcProxy(PLC_REMOTE_IP, PLC_PORT, XWAY_REMOTE_STATION);

  assert(plc != NULL);

  // create 2 threads
  pthread_t thread1;
  pthread_t thread2;

  struct train_thread_attr attr1;
  attr1.routeFilePath = routeFilePath1;
  attr1.station = XWAY_HOST_STATION1;

  struct train_thread_attr attr2;
  attr2.routeFilePath = routeFilePath2;
  attr2.station = XWAY_HOST_STATION2;

  pthread_create(&thread1, NULL, (void*(*)(void*))trainThread, &attr1);
  pthread_create(&thread2, NULL, (void*(*)(void*))trainThread, &attr2);

  // wait for threads to end, even if they should be infinite loops
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  endPlcProxy(plc);
  endResourceManagerProxy(resManager);
  return 0;
}

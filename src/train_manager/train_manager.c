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
  verbose("\n[TRAIN MANAGER]: Ctrl-C Captured. Exiting Program \n");

  if (resManager != NULL) {
    (void)endResourceManagerProxy(resManager);
  }

  if (plc != NULL) {
    (void)endPlcProxy(plc);
  }

  exit(0);
}

struct train_thread_attr {
  char* routeFilePath;
  int id;
};

/**
 * @brief Entry point for the train thread
 * @param[in] data Pointer to the Train_t instance
 * @return Thread exit status (always NULL)
 */
void* trainThread(struct train_thread_attr* attr) {
  verbose("[TrainThread][%s][%d] Initializing ... \n", attr->routeFilePath,
          attr->id);
  Train_t* train = initTrain(plc, resManager, attr->routeFilePath);
  assert(train != NULL);

  // TODO set Train IDs
  setTrainId(train, attr->id);
  int execRes = executeRoute(train, XWAY_HOST_STATION);
  sleep(3);

  assert(execRes == 0);

  endTrain(train);
  endResourceManagerProxy(resManager);
  endPlcProxy(plc);

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

  char* trainIdStr1;
  if (!get_flag_value(argc, argv, "--train1", &trainIdStr1)) {
    verbose("[Train Manager] no train1 specified\n");
    exit(EXIT_FAILURE);
  }
  const int trainId1 = atoi(trainIdStr1);
  verbose("[Train Manager]: using train1 %d \n", trainId1);

  char* trainIdStr2;
  if (!get_flag_value(argc, argv, "--train2", &trainIdStr2)) {
    verbose("[Train Manager] no train2 specified\n");
    exit(EXIT_FAILURE);
  }
  const int trainId2 = atoi(trainIdStr2);
  verbose("[Train Manager]: using train2 %d \n", trainId2);

  verbose("[Train Manager] connecting to ressource manager\n");
  resManager =
      initResourceManagerProxy(RES_MANAGER_REMOTE_IP, RES_MANAGER_PORT);
  assert(resManager != NULL);

  verbose("[Train Manager] connecting to plc\n");
  plc = initPlcProxy(HOST_IP, PLC_REMOTE_IP, PLC_PORT);
  int netRes = setXwayAddrs(plc, XWAY_HOST_STATION, XWAY_REMOTE_STATION,
                            XWAY_NETWORK, XWAY_PORT);

  assert(plc != NULL);
  assert(netRes == 0);

  // create 2 threads
  pthread_t thread1;
  pthread_t thread2;

  struct train_thread_attr attr1;
  attr1.routeFilePath = routeFilePath1;
  attr1.id = trainId1;

  struct train_thread_attr attr2;
  attr2.routeFilePath = routeFilePath2;
  attr2.id = trainId2;

  pthread_create(&thread1, NULL, (void* (*)(void*))trainThread, &attr1);
  pthread_create(&thread2, NULL, (void* (*)(void*))trainThread, &attr2);

  // wait for threads to end, even if they should be infinite loops
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  return 0;

  exit(EXIT_SUCCESS);
}

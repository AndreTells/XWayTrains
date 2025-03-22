#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "plc/plc_message.h"
#include "plc/plc_proxy.h"

#define MAX_NUM_REGISTRABLE_TRAINS 4

struct PlcProxy_t {
  pthread_t readerThreadTid;
  sem_t mutex;
  int outputFd[MAX_NUM_REGISTRABLE_TRAINS][2];
  bool finished;
  int sock_fd;
};

PlcProxy_t* initPlcProxy(char* hostIpAddr, char* plcIpAddr, int port){
  // check if it's a valid IP address
  if (plcIpAddr == NULL) {
    return NULL;
  }

  PlcProxy_t* plcProxy = (PlcProxy_t*)malloc(sizeof(PlcProxy_t));

  plcProxy->finished = false;

  return plcProxy;
}

int endPlcProxy(PlcProxy_t* plc) {
  if (plc == NULL) {
    return -1;
  }

  plc->finished = true;

  free(plc);
  return 0;
}

int sendMessagePlcProxy(PlcProxy_t* plc, PlcMessage_t* msg) {
  return 0;
}

PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, int clientId) {
  return createPlcMessage();
}

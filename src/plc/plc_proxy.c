#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "plc/plc_message.h"
#include "plc/plc_proxy.h"
#include "common/verbose.h"

#define MAX_NUM_REGISTRABLE_TRAINS 4

struct PlcProxy_t {
  pthread_t readerThreadTid;
  sem_t mutex;
  int outputFd[MAX_NUM_REGISTRABLE_TRAINS][2];
  bool finished;
  int sock_fd;
};

/**
 * @brief Entry point for the PLC message receiver thread
 * @param[in] plcProxy Pointer to the PlcProxy_t instance
 * @return Thread exit status (always NULL)
 */
void* plcProxyMsgReceiverThread(void* plcProxy);

/**
 * @brief Attempt to register a client with the plc proxy
 * @param[in] resManager Proxy instance handle
 * @param[in] clientId ID of the client to register
 * @return 0 on success, non-zero error code on failure
 */
int plcProxyTryRegisterClient(PlcProxy_t* plcProxy, int clientId);

PlcProxy_t* initPlcProxy(char* plcIpAddr) {
  // check if it's a valid IP address
  verbose("[PLC PROXY]: Initializing ... \n");
  if (plcIpAddr == NULL) {
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  PlcProxy_t* plcProxy = (PlcProxy_t*)malloc(sizeof(PlcProxy_t));

  if(plcProxy == NULL){
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  // TODO(felipe): connect to remote plc

  plcProxy->finished = false;

  // Initialize the output file descriptors to -1 (invalid)
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    plcProxy->outputFd[i][0] = -1;
    plcProxy->outputFd[i][1] = -1;
  }

  int resSemInit = sem_init(&(plcProxy->mutex), 0, 1);
  // check if sem_init failed
  if (resSemInit != 0) {
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  // create reader thread
  verbose("[PLC PROXY]: Initializing Reader Thread ... \n");
  int resPthreadCreate =
      pthread_create(&(plcProxy->readerThreadTid), NULL,
                     plcProxyMsgReceiverThread, (void*)plcProxy);

  // check if pthread_create
  if (resPthreadCreate != 0) {
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing Reader Thread ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  return plcProxy;
}

int endPlcProxy(PlcProxy_t* plc) {
  verbose("[PLC PROXY]: Ending ... \n");
  if (plc == NULL) {
    verbose("[PLC PROXY]: Ending ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  verbose("[PLC PROXY]: Ending Reader Thread ... \n");
  plc->finished = true;
  int retVal = 0;
  pthread_kill(plc->readerThreadTid, SIGINT);

  (void)pthread_join(plc->readerThreadTid, (void*)&retVal);
  // check if the join failed
  if (retVal != 0) {
    verbose("[PLC PROXY]: Ending Reader Thread ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[PLC PROXY]: Ending ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  verbose("[PLC PROXY]: Ending Reader Thread ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  sem_destroy(&(plc->mutex));

  // closing all open pipes
  verbose("[PLC PROXY]:Closing Pipes ... \n");
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    if (plc->outputFd[i][0] == -1) {
      continue;
    }
    close(plc->outputFd[i][0]);
    close(plc->outputFd[i][1]);
  }

  verbose("[PLC PROXY]: Closing Pipes ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  free(plc);
  return 0;
}

int sendMessagePlcProxy(PlcProxy_t* plc, PlcMessage_t* msg) {
  sem_wait(&(plc->mutex));
  printf("msg %x sent to plc\n", msg);
  sem_post(&(plc->mutex));
  return 0;
}

PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, int clientId) {
  if (plcProxyTryRegisterClient(plc, clientId) != 0) {
    return NULL;
  }

  uint8_t* serMsg[] = malloc(MAX_MSG_SIZE);

  int resWait = fileDescriptorTimedWait(plc->outputFd[clientId][0]);
  if(res < 0){
    return NULL;
  }

  ssize_t res_size = read(plc->outputFd[clientId][0], serMsg, MAX_MSG_SIZE);
  if (res_size == 0) {
    return NULL;
  }

  PlcMessage_t* msg = deserializedMsg(serMsg);

  PlcMessage_t* ack = createACK(msg);

  uint8_t* serAck[] = malloc(MAX_MSG_SIZE);


  return msg;
}

void* plcProxyMsgReceiverThread(void* plcProxy) {
  PlcProxy_t* plc = (PlcProxy_t*)plcProxy;
  while (!plc->finished) {
    printf("plc proxy attempting to get a line: \n");
    // will be switched with code to read a real PlcMessage_t
    // AND not an ack
    int target;
    int resp;
    (void)scanf("%d,%d", &target, &resp);
    // generate response message
    (void)printf("[ACK]reader read: %d %d\n", target, resp);

    if (plcProxyTryRegisterClient(plc, target) != 0) {
      // TODO(andre): treat silent error ??
      continue;
    }
    printf("outputing to %d\n", plc->outputFd[target][1]);
    PlcMessage_t* msg = getNullMessage();  // use actually received message
    (void)write(plc->outputFd[target][1], msg, getMessageSize(msg));
    free(msg);
    sleep(5);
  }

  pthread_exit(NULL);
}

int plcProxyTryRegisterClient(PlcProxy_t* plcProxy, int clientId) {
  // index out of range
  if (clientId < 0 || clientId > MAX_NUM_REGISTRABLE_TRAINS - 1) {
    return -1;
  }
  if (plcProxy->outputFd[clientId][0] != -1) {
    return 0;
  }

  int status = pipe(plcProxy->outputFd[clientId]);

  return status;
}

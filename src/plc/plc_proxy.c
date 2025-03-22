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
#include "common/time_out.h"
#include "common/comm_general.h"

#define MAX_NUM_REGISTRABLE_TRAINS 5 // ignore position 0

struct PlcProxy_t {
  pthread_t readerThreadTid;
  sem_t mutex;
  int outputFd[MAX_NUM_REGISTRABLE_TRAINS][2];
  bool finished;
  int sock_fd;
};

PlcMessage_t* tryGetPlcMessage(int fd);

int sendPlcMessageToFd(PlcMessage_t* msg, int fd);

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

PlcProxy_t* initPlcProxy(char* hostIpAddr, char* plcIpAddr, int port) {
  // check if it's a valid IP address
  verbose("[PLC PROXY]: Initializing ... \n");
  if (plcIpAddr == NULL || hostIpAddr == NULL || port < 0) {
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  PlcProxy_t* plcProxy = (PlcProxy_t*)malloc(sizeof(PlcProxy_t));

  if(plcProxy == NULL){
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  plcProxy->sock_fd = tcpCreateSocketWrapper(false, hostIpAddr, port);

  if(plcProxy->sock_fd == -1){
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  int connRes = tcpConnectWrapper(plcProxy->sock_fd, plcIpAddr, port);

  if(connRes == -1){
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

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

  (void)pthread_join(plc->readerThreadTid, NULL);

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

  sendPlcMessageToFd(msg, plc->sock_fd);

  sem_post(&(plc->mutex));

  return 0;
}

PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, int clientId) {
  if (plcProxyTryRegisterClient(plc, clientId) != 0) {
    return NULL;
  }

  PlcMessage_t* msg = tryGetPlcMessage(plc->outputFd[clientId][0]);
  if(msg == NULL){
    return msg;
  }

  PlcMessage_t* ack = createACK(msg, true);

  int sendRes = sendMessagePlcProxy(plc, ack);
  free(ack);

  if(sendRes < -1) {
    free(msg);
    return NULL;
  }

  return msg;
}

void* plcProxyMsgReceiverThread(void* plcProxy) {
  PlcProxy_t* plc = (PlcProxy_t*)plcProxy;
  while (!plc->finished) {
    printf("plc proxy attempting to get a line: \n");

    PlcMessage_t* msg = tryGetPlcMessage(plc->sock_fd);

    // if failed to get a msg, retry
    if(msg == NULL){
      continue;
    }

    // things that aren't write
    if(!compareMsgType(msg, APDU_WRITE_RESP)){
      continue;
    }

    // determining who to route the message to
    uint8_t* data = getPlcMessageData(msg);
    TrainId_e target = getTargetedTrain(data);

    if (plcProxyTryRegisterClient(plc, target) == 0) {
      sendPlcMessageToFd(msg, plc->outputFd[target][1]);
    }

    free(msg);
  }

  pthread_exit(NULL);
}

int plcProxyTryRegisterClient(PlcProxy_t* plcProxy, int clientId) {
  // index out of range
  if (clientId < 0 || clientId > MAX_NUM_REGISTRABLE_TRAINS - 1) {
    return -1;
  }
  // check if client is already registered
  if (plcProxy->outputFd[clientId][0] != -1) {
    return 0;
  }

  int status = pipe(plcProxy->outputFd[clientId]);

  return status;
}

PlcMessage_t* tryGetPlcMessage(int fd){
  uint8_t serMsg[MAX_MSG_SIZE];

  int resWait = fileDescriptorTimedWait(fd);
  if(resWait < 0){
    return NULL;
  }

  ssize_t resSize = read(fd, serMsg, MAX_MSG_SIZE);
  if (resSize == 0) {
    return NULL;
  }

  PlcMessage_t* msg = deserializePlcMessage(serMsg);
  return msg;
}

// does not consider the situation where it failed to connect to a server
int sendPlcMessageToFd(PlcMessage_t* msg, int fd){
  uint8_t serMsg[MAX_MSG_SIZE];

  size_t serSize = serializePlcMessage(msg, serMsg);

  int writeRes = write(fd, serMsg, serSize);

  return writeRes;
}

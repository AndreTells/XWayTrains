#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/resource_request.h"
#include "common/verbose.h"
#include "plc/model_info.h"
#include "train_manager/resource_manager_proxy.h"

#define MAX_NUM_REGISTRABLE_TRAINS 4

/**
 * @brief Structure representing a Resource Manager Proxy instance
 * @details Encapsulates all state and functionality for communicating with the
 * remote resource manager.
 */
struct ResourceManagerProxy_t {
  pthread_t readerThreadTid;
  sem_t mutex;
  int outputFd[MAX_NUM_REGISTRABLE_TRAINS][2];
  bool finished;
  int sock_fd;
};

void* resManagerMsgReceiverThread(void* resourceManagerProxy);

int resManagerTryRegisterClient(ResourceManagerProxy_t* resManager,
                                int clientId);

ResourceManagerProxy_t* initResourceManagerProxy(char* resManagerIpAddr) {
  // check if it's a valid IP address
  verbose("[RESOURCE MANAGER PROXY]: Initializing ... \n");
  if (resManagerIpAddr == NULL) {
    verbose("[RESOURCE MANAGER PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  ResourceManagerProxy_t* resManager =
      (ResourceManagerProxy_t*)malloc(sizeof(ResourceManagerProxy_t));

  // check if malloc failed
  if (resManager == NULL) {
    verbose("[RESOURCE MANAGER PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return NULL;
  }

  //TODO: create socket & connect

  resManager->finished = false;

  // Initialize the output file descriptors to -1 (invalid)
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    resManager->outputFd[i][0] = -1;
    resManager->outputFd[i][1] = -1;
  }

  int resSemInit = sem_init(&(resManager->mutex), 0, 1);
  // check if sem_init failed
  if (resSemInit != 0) {
    verbose("[RESOURCE MANAGER PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    free(resManager);
    return NULL;
  }

  verbose("[RESOURCE MANAGER PROXY]: Initializing Reader Thread ... \n");
  // create reader thread
  int resPthreadCreate =
      pthread_create(&(resManager->readerThreadTid), NULL,
                     resManagerMsgReceiverThread, (void*)resManager);
  // check if pthread_create
  if (resPthreadCreate != 0) {
    verbose("[RESOURCE MANAGER PROXY]: Initializing Reader Thread ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[RESOURCE MANAGER PROXY]: Initializing ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    free(resManager);
    return NULL;
  }

  verbose("[RESOURCE MANAGER PROXY]: Initializing Reader Thread ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  verbose("[RESOURCE MANAGER PROXY]: Initializing ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return resManager;
}

int endResourceManagerProxy(ResourceManagerProxy_t* resManager) {
  verbose("[RESOURCE MANAGER PROXY]: Ending ... \n");
  if (resManager == NULL) {
    return -1;  // Return error if resManager is NULL
  }

  verbose("[RESOURCE MANAGER PROXY]: Joining Reader Thread ... \n");
  resManager->finished = true;
  int retVal = pthread_join(resManager->readerThreadTid, NULL);
  // check if the join failed
  if (retVal != 0) {
    verbose("[RESOURCE MANAGER PROXY]: Joining Reader Thread ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  sem_destroy(&(resManager->mutex));

  verbose("[RESOURCE MANAGER PROXY]: Joining Reader Thread ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  // closing all open pipes
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {

    if (resManager->outputFd[i][0] == -1) {
      continue;
    }

    verbose("[RESOURCE MANAGER PROXY]: Closing pipe ... \n");
    close(resManager->outputFd[i][0]);
    close(resManager->outputFd[i][1]);

    verbose("[RESOURCE MANAGER PROXY]: Closing pipe ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  }

  // TODO(andre): end tcp connection (?)

  free(resManager);
  verbose("[RESOURCE MANAGER PROXY]: Ending ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return 0;
}

int requestResource(ResourceManagerProxy_t* resManager,
                    ResourceRequestType_e reqType, int resourceId,
                    int clientId){
  int res;
  verbose("[RESOURCE MANAGER PROXY]: Resource Request ... \n");
  verbose("[RESOURCE MANAGER PROXY]: Checking if client is registered ... \n");
  if (resManagerTryRegisterClient(resManager, clientId) != 0) {
    verbose("[RESOURCE MANAGER PROXY]: Checking if client is registered ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[RESOURCE MANAGER PROXY]: Resource Request ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  verbose("[RESOURCE MANAGER PROXY]: Checking if client is registered ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  ResourceRequest_t* req = createResourceRequest(clientId, resourceId, reqType, -1 );

  verbose("[RESOURCE MANAGER PROXY]: Sending Request ... \n");
  sem_wait(&(resManager->mutex));

  res = sendResourceRequest(resManager->sock_fd, req);
  if(res < 0){
    verbose("[RESOURCE MANAGER PROXY]: Sending Request ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[RESOURCE MANAGER PROXY]: Resource Request ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  // send message via socket

  sem_post(&(resManager->mutex));
  verbose("[RESOURCE MANAGER PROXY]: Sending Request ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[RESOURCE MANAGER PROXY]: Waiting For Response ... \n");
  ResourceRequestResponse_t* resp = recvResourceRequestResponse(resManager->outputFd[clientId][0]);

  if (resp == NULL){
    verbose("[RESOURCE MANAGER PROXY]: Waiting For Response ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[RESOURCE MANAGER PROXY]: Resource Request ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  verbose("[RESOURCE MANAGER PROXY]: Waiting For Response ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  free(req);
  if(resp->respType != RESOURCE_GRANTED){
    verbose("[RESOURCE MANAGER PROXY]: Resource Request ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  free(resp);

  verbose("[RESOURCE MANAGER PROXY]: Resource Request ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return 0;
}

void* resManagerMsgReceiverThread(void* resourceManagerProxy) {
  ResourceManagerProxy_t* resManager =
      (ResourceManagerProxy_t*)resourceManagerProxy;
  int fd = resManager->sock_fd;

  while (!resManager->finished) {
    verbose("[RESOURCE MANAGER PROXY READER]: Waiting For a Message ... \n");

    ResourceRequestResponse_t* resp = recvResourceRequestResponse(fd);
    if(resp == NULL){
      verbose("[RESOURCE MANAGER PROXY READER]: Waiting For a Message Timed Out or Error  \n");
      continue;
    }

    verbose("[RESOURCE MANAGER PROXY READER]: Message Received \n");
    verbose("[RESOURCE MANAGER PROXY READER]: Routing Message ... \n");
    int target = resp->requesterId;

    if (resManagerTryRegisterClient(resManager, target) != 0) {
      // TODO(andre): treat silent error ??
      verbose("[RESOURCE MANAGER PROXY READER]: Routing Message ...  " VERBOSE_KRED "fail \n" VERBOSE_RESET);
      free(resp);
      continue;
    }

    answerResourceRequest(resManager->outputFd[target][1], resp);
    free(resp);
    verbose("[RESOURCE MANAGER PROXY READER]: Routing Message ...  " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  }

  pthread_exit(NULL);
}

int resManagerTryRegisterClient(ResourceManagerProxy_t* resManager,
                                int clientId) {
  // index out of range
  if (clientId < 0 || clientId > MAX_NUM_REGISTRABLE_TRAINS - 1) {
    return -1;
  }
  if (resManager->outputFd[clientId][0] != -1) {
    return 0;
  }

  int status = pipe(resManager->outputFd[clientId]);

  return status;
}

#include "resource_manager/resource_manager.h"


#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/comm_general.h"
#include "common/resource_request.h"
#include "common/verbose.h"
#include "resource_manager/request_queue.h"

#define MAX_CLIENTS 4

struct ResourceManager_t {
  bool finished;
  int listenFd;
  ResourceDataBaseProxy_t* safeDatabase;

  ResourceRequestQueue_t* queue;

  pthread_t consumerThread;

  pthread_t clients[MAX_CLIENTS];
  int clientsFd[MAX_CLIENTS];
  int lastClientIndex;
};

typedef struct{
  int inputFd; // were it's getting the requests from
  ResourceManager_t* parent;
  ResourceRequestQueue_t* queue;
}RequestProducerThread_t;

typedef struct{
  ResourceManager_t* parent;
  ResourceRequestQueue_t* queue;
}RequestConsumerThread_t;

void* producerThread(void* data);

void* consumerThread(void* data);

ResourceManager_t* initResourceManager(ResourceDataBaseProxy_t* safeDatabase,
                                       char* ipAddress, int port) {
  ResourceManager_t* manager =
      (ResourceManager_t*)malloc(sizeof(ResourceManager_t));
  // not checking for error in malloc

  manager->safeDatabase = safeDatabase;
  manager->lastClientIndex = -1;
  manager->finished = false;
  manager->queue = initQueue();

  // opening socket
  int listenFd = tcpCreateSocketWrapper(true, ipAddress, port);

  if (listenFd < 0) {
    free(manager);
    return NULL;
  }

  manager->listenFd = listenFd;

  // create consumer thread
  RequestConsumerThread_t* typedData = (RequestConsumerThread_t*) malloc(sizeof(RequestConsumerThread_t));
  typedData->parent = manager;
  typedData->queue = manager->queue;

  pthread_create(&(manager->consumerThread),NULL,consumerThread, (void*)typedData);

  return manager;
}

int endResourceManager(ResourceManager_t* manager) {
  manager->finished = true;

  pthread_join(manager->consumerThread, NULL);

  for(int i=0; i<manager->lastClientIndex; i++){
    (void)pthread_join(manager->clients[i], NULL);
    // not checking if the join is failing
  }

  (void)destroyQueue(manager->queue);
  close(manager->listenFd);

  (void)free(manager);
  return 0;
}

int acceptTrainManager(ResourceManager_t* manager) {
  int connectionFd = tcpAcceptWrapper(manager->listenFd);

  if (connectionFd < 0) {
    return -1;
  }
  manager->lastClientIndex +=1;
  int i = manager->lastClientIndex; // id of the client

  manager->clientsFd[i] = connectionFd;
  RequestProducerThread_t* threadData = malloc(sizeof(RequestProducerThread_t*));
  threadData->inputFd = connectionFd;
  threadData->parent = manager;
  threadData->queue = manager->queue;

  pthread_create(&(manager->clients[i]),NULL,producerThread, (void*)threadData);

  return 0;
}

void* producerThread(void* data) {
  RequestProducerThread_t* typedData = (RequestProducerThread_t*) data;
  ResourceManager_t* manager = typedData->parent;
  ResourceRequestQueue_t* queue = typedData->queue;
  int inputFd = typedData->inputFd;

  while(!manager->finished){
    ResourceRequest_t* req = recvResourceRequest(inputFd);
    pushQueue(queue,req);
  }

  free(typedData);
  pthread_exit(NULL);
}

void* consumerThread(void* data) {
  RequestConsumerThread_t* typedData = (RequestConsumerThread_t*) data;
  ResourceManager_t* manager = typedData->parent;
  ResourceRequestQueue_t* queue = typedData->queue;

  while(!manager->finished){
    ResourceRequest_t* req = popQueue(queue);

    if(req == NULL){
      continue;
    }

    int res = -1;
    switch(req->reqType){
      case LOCK_RESOURCE:
        (void)waitResourceProxy(manager->safeDatabase, req->resourceId);
        (void)attemptLockResourceProxy(manager->safeDatabase, req->resourceId, req->requesterId);
        res = 0;
        break;

      case RELEASE_RESOURCE:
        (void)releaseResourceProxy(manager->safeDatabase, req->resourceId, req->requesterId);
        res = 0;
        break;
    }

    answerResourceRequest(req, res);
  }

  free(typedData);
  pthread_exit(NULL);
}

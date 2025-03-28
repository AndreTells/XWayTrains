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

#define MAX_CLIENTS 50
#define THREAD_POOL_SIZE 4

struct ResourceManager_t {
  bool finished;
  int listenFd;
  ResourceDataBaseProxy_t* safeDatabase;

  ResourceRequestQueue_t* queue;

  pthread_t consumerThreadPool[THREAD_POOL_SIZE];

  pthread_t clients[MAX_CLIENTS];
  int clientsFd[MAX_CLIENTS];
  int lastClientIndex;
};

typedef struct {
  int inputFd;  // were it's getting the requests from
  ResourceManager_t* parent;
  ResourceRequestQueue_t* queue;
} RequestProducerThread_t;

typedef struct {
  ResourceManager_t* parent;
  ResourceRequestQueue_t* queue;
} RequestConsumerThread_t;

void* producerThread(void* data);

void* consumerThread(void* data);

ResourceManager_t* initResourceManager(ResourceDataBaseProxy_t* safeDatabase,
                                       char* ipAddress, const uint16_t port) {
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

  for (int i = 0; i < THREAD_POOL_SIZE; i++) {
    // create consumer thread
    RequestConsumerThread_t* typedData =
        (RequestConsumerThread_t*)malloc(sizeof(RequestConsumerThread_t));
    typedData->parent = manager;
    typedData->queue = manager->queue;
    pthread_create(&(manager->consumerThreadPool[i]), NULL, consumerThread,
                   (void*)typedData);
  }

  return manager;
}

int endResourceManager(ResourceManager_t* manager) {
  // considering edge cases
  if (manager == NULL) {
    return -1;
  }
  manager->finished = true;

  for (int i = 0; i < THREAD_POOL_SIZE; i++) {
    int res = pthread_join(manager->consumerThreadPool[i], NULL);
    if (res < 0) {
      return -1;
    }
  }

  for (int i = 0; i < manager->lastClientIndex + 1; i++) {
    int res = pthread_join(manager->clients[i], NULL);
    if (res < 0) {
      return -1;
    }
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
  manager->lastClientIndex += 1;
  int i = manager->lastClientIndex;  // id of the client

  manager->clientsFd[i] = connectionFd;
  RequestProducerThread_t* threadData = malloc(sizeof(RequestProducerThread_t));
  threadData->inputFd = connectionFd;
  threadData->parent = manager;
  threadData->queue = manager->queue;

  pthread_create(&(manager->clients[i]), NULL, producerThread,
                 (void*)threadData);

  return 0;
}

void* producerThread(void* data) {
  verbose("[RESOURCE MANAGER]: initializing producer thread\n");
  RequestProducerThread_t* typedData = (RequestProducerThread_t*)data;
  ResourceManager_t* manager = typedData->parent;

  if (manager == NULL) {
    free(typedData);
    pthread_exit(NULL);
  }

  ResourceRequestQueue_t* queue = typedData->queue;

  if (queue == NULL) {
    free(typedData);
    pthread_exit(NULL);
  }

  int inputFd = typedData->inputFd;

  verbose("[RESOURCE MANAGER]: Waiting for messages in socket %d\n", inputFd);
  while (!(manager->finished)) {
    ResourceRequest_t* req = recvResourceRequest(inputFd);

    if (req == NULL) {
      continue;
    }

    // client disconnected
    if (req->reqType == UNKNOWN_RES_REQ_TYPE) {
      verbose("[RESOURCE MANAGER]: Client Disconnected or Unkwon message\n");
      break;
    }

    verbose(
        "[RESOURCE MANAGER]: Producer Thread received a message from %d "
        "request type %d for resource %d\n",
        req->requesterId, req->reqType, req->resourceId);

    pushQueue(queue, req);
  }

  close(inputFd);
  free(typedData);
  pthread_exit(NULL);
}

void* consumerThread(void* data) {
  verbose("[RESOURCE MANAGER]: initializing consumer thread\n");
  RequestConsumerThread_t* typedData = (RequestConsumerThread_t*)data;
  ResourceManager_t* manager = typedData->parent;
  ResourceRequestQueue_t* queue = typedData->queue;

  ResourceRequest_t* req = NULL;
  bool timedOut = false;
  while (!manager->finished) {
    if (!timedOut) {
      req = popQueue(queue);
    }

    if (req == NULL) {
      continue;
    }

    verbose("[RESOURCE MANAGER]: Consumer thread got a message\n");

    timedOut = false;
    ResourceRequestResponseType_e res = RESOURCE_REFUSED;
    switch (req->reqType) {
      case LOCK_RESOURCE:
        int resWait = waitResourceProxy(manager->safeDatabase, req->resourceId);
        if (resWait != 0) {
          verbose(
              "[RESOURCE MANAGER]: consumer thread timed out waiting for "
              "resource\n");
          timedOut = true;
          break;
        }

        int resLock = attemptLockResourceProxy(
            manager->safeDatabase, req->resourceId, req->requesterId);

        if (resLock == 0) {
          verbose("[RESOURCE MANAGER]: consumer thread locked resource %d\n",
                  req->resourceId);
          res = RESOURCE_GRANTED;
        }

        break;

      case RELEASE_RESOURCE:
        int resRel = releaseResourceProxy(manager->safeDatabase,
                                          req->resourceId, req->requesterId);
        if (resRel == 0) {
          verbose("[RESOURCE MANAGER]: consumer thread released resource %d\n",
                  req->resourceId);
          res = RESOURCE_GRANTED;
        }
        break;
      case UNKNOWN_RES_REQ_TYPE:
        timedOut = true;
        manager->finished = true;
        break;
    }

    if (timedOut) {
      verbose(
          "[RESOURCE MANAGER]: consumer thread timed out treating message\n");
      continue;
    }

    int fd = req->returnFd;
    ResourceRequestResponse_t* resp = createResourceRequestResponse(req, res);
    answerResourceRequest(fd, resp);
    verbose(
        "[RESOURCE MANAGER]: consumer thread responded request via socket %d\n",
        fd);
  }

  free(typedData);
  pthread_exit(NULL);
}

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

/**
 * @brief Entry point for the resource manager message receiver thread
 * @param[in] resourceManagerProxy Pointer to the ResourceManagerProxy_t
 * instance
 * @return Thread exit status (always NULL)
 */
void* resManagerMsgReceiverThread(void* resourceManagerProxy);

/**
 * @brief Attempt to register a client with the resource manager
 * @param[in] resManager Proxy instance handle
 * @param[in] clientId ID of the client to register
 * @return 0 on success, non-zero error code on failure
 */
int resManagerTryRegisterClient(ResourceManagerProxy_t* resManager,
                                int clientId);

/**
 * @brief Initialize a new Resource Manager Proxy instance
 * @param[in] resManagerIpAddr IP address of the resource Manager
 * @return Pointer to the newly created ResourceManagerProxy_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endResourceManagerProxy()`.
 */
ResourceManagerProxy_t* initResourceManagerProxy(char* resManagerIpAddr) {
  // check if it's a valid IP address
  if (resManagerIpAddr == NULL) {
    return NULL;
  }

  ResourceManagerProxy_t* resManager =
      (ResourceManagerProxy_t*)malloc(sizeof(ResourceManagerProxy_t));

  // check if malloc failed
  if (resManager == NULL) {
    return NULL;
  }

  resManager->finished = false;

  // Initialize the output file descriptors to -1 (invalid)
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    resManager->outputFd[i][0] = -1;
    resManager->outputFd[i][1] = -1;
  }

  int resSemInit = sem_init(&(resManager->mutex), 0, 1);
  // check if sem_init failed
  if (resSemInit != 0) {
    free(resManager);
    return NULL;
  }

  // create reader thread
  int resPthreadCreate =
      pthread_create(&(resManager->readerThreadTid), NULL,
                     resManagerMsgReceiverThread, (void*)resManager);
  // check if pthread_create
  if (resPthreadCreate != 0) {
    free(resManager);
    return NULL;
  }

  return resManager;
}

/**
 * @brief Gracefully terminate a Resource Manager Proxy instance
 * @param[in] resManager Proxy instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endResourceManagerProxy(ResourceManagerProxy_t* resManager) {
  if (resManager == NULL) {
    return -1;  // Return error if resManager is NULL
  }

  resManager->finished = true;
  int retVal = 0;
  pthread_kill(resManager->readerThreadTid, SIGINT);
  (void)pthread_join(resManager->readerThreadTid, (void*)&retVal);
  // check if the join failed
  if (retVal != 0) {
    return -1;
  }

  sem_destroy(&(resManager->mutex));

  // closing all open pipes
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    if (resManager->outputFd[i][0] == -1) {
      continue;
    }
    close(resManager->outputFd[i][0]);
    close(resManager->outputFd[i][1]);
  }

  // TODO(andre): end tcp connection (?)

  free(resManager);
  return 0;
}

/**
 * @brief Request a resource from the remote manager
 * @param[in] resManager Proxy instance handle
 * @param[in] resourceId ID of the resource to request
 * @param[in] clientId ID of the client requesting the resource
 * @return 0 on success, non-zero error code on failure
 */
int requestResource(ResourceManagerProxy_t* resManager, int ressourceId,
                    int clientId) {
  if (resManagerTryRegisterClient(resManager, clientId) != 0) {
    return -1;
  }

  sem_wait(&(resManager->mutex));

  printf("%d requested: %d\n", clientId, ressourceId);

  sem_post(&(resManager->mutex));

  int res = 0;
  ssize_t res_size = read(resManager->outputFd[clientId][0], &res, sizeof(int));
  if (res_size == 0) {
    return -1;
  }
  printf("%d received %d with size %d from %d \n", clientId, res, res_size,
         resManager->outputFd[clientId][0]);

  return 0;
}

/**
 * @brief Release a previously allocated resource
 * @param[in] resManager Proxy instance handle
 * @param[in] resourceId ID of the resource to release
 * @param[in] clientId ID of the client releasing the resource
 * @return 0 on success, non-zero error code on failure
 */
int releaseResource(ResourceManagerProxy_t* resManager, int ressourceId,
                    int clientId) {
  if (resManagerTryRegisterClient(resManager, clientId) != 0) {
    return -1;
  }

  sem_wait(&(resManager->mutex));

  printf("%d tried to release: %d\n", clientId, ressourceId);

  sem_post(&(resManager->mutex));

  int res = 0;
  ssize_t res_size = read(resManager->outputFd[clientId][0], &res, sizeof(int));
  if (res_size == 0) {
    return -1;
  }
  printf("%d received %d with size %d from %d \n", clientId, res, res_size,
         resManager->outputFd[clientId][0]);

  return 0;
}

/**
 * @brief Entry point for the resource manager message receiver thread
 * @param[in] resourceManagerProxy Pointer to the ResourceManagerProxy_t
 * instance
 * @return Thread exit status (always NULL)
 */
void* resManagerMsgReceiverThread(void* resourceManagerProxy) {
  ResourceManagerProxy_t* resManager =
      (ResourceManagerProxy_t*)resourceManagerProxy;

  printf("reader thread initialized\n");
  while (!resManager->finished) {
    printf("res manager attempting to get a line: \n");
    int target;
    int resp;
    (void)scanf("%d,%d", &target, &resp);
    (void)printf("reader read: %d %d\n", target, resp);

    if (resManagerTryRegisterClient(resManager, target) != 0) {
      // TODO(andre): treat silent error ??
      continue;
    }
    printf("outputing to %d\n", resManager->outputFd[target][1]);
    (void)write(resManager->outputFd[target][1], &resp, sizeof(int));
    sleep(5);
  }

  pthread_exit(NULL);
}

/**
 * @brief Attempt to register a client with the resource manager
 * @param[in] resManager Proxy instance handle
 * @param[in] clientId ID of the client to register
 * @return 0 on success, non-zero error code on failure
 */
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

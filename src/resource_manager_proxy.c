#include "resource_manager_proxy.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NUM_REGISTRABLE_TRAINS 4;

/**
 * @brief Structure representing a Resource Manager Proxy instance
 * @details Encapsulates all state and functionality for communicating with the
 * remote resource manager.
 */
struct ResourceManagerProxy_t {
  pthread_t readerThreadTid;
  semaphore_t mutex;
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

  // TODO(andre): connect to remote ressource manager

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
  (void)pthread_join(resManager->readerThreadTid, &retVal);
  // check if the join failed
  if (retVal != 0) {
    return -1;
  }

  sem_destroy(&(resManager->mutex));

  // closing all open pipes
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    if (plcProxy->outputFd[i][0] == -1) {
      continue;
    }
    close(plcProxy->outputFd[i][0]);
    close(plcProxy->outputFd[i][1]);
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
  sem_wait(&(resManager->mutex));

  // TODO(andre): send message ressource manager
  sleep(10);

  sem_pos(&(resManager->mutex));
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
  sem_wait(&(resManager->mutex));

  // TODO(andre): do the following
  // build message
  // TODO(andre): send message ressource manager
  // read from file descriptor
  sleep(10);

  sem_post(&(resManager->mutex));
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
  while (!resManager->finished) {
    // TODO(andre): do the following
    // read from socket
    // check the target is registered
    // copy message to correct output file descriptor
  }
  pthread_exit(NULL);
}

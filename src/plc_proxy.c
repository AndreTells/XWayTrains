#include "plc_proxy.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NUM_REGISTRABLE_TRAINS 4

/**
 * @brief Structure representing a PLC Proxy instance
 * @details Encapsulates all state and functionality for communicating with the
 * PLC.
 */
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
 * @brief Initialize a new PLC Proxy instance
 * @param plcIpAddr IP Address of the plc
 * @return Pointer to the newly created PlcProxy_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endPlcProxy()`.
 */
PlcProxy_t* initPlcProxy(char* plcIpAddr) {
  // check if it's a valid IP address
  if (plcIpAddr == NULL) {
    return NULL;
  }

  PlcProxy_t* plcProxy = (PlcProxy_t*)malloc(sizeof(PlcProxy_t));

  // TODO(felipe): connect to remote plc

  plcProxy->finished = true;

  // Initialize the output file descriptors to -1 (invalid)
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    plcProxy->outputFd[i][0] = -1;
    plcProxy->outputFd[i][1] = -1;
  }

  int resSemInit = sem_init(&(plcProxy->mutex), 0, 1);
  // check if sem_init failed
  if (resSemInit != 0) {
    free(plcProxy);
    return NULL;
  }

  // create reader thread
  int resPthreadCreate =
      pthread_create(&(plcProxy->readerThreadTid), NULL,
                     plcProxyMsgReceiverThread, (void*)plcProxy);

  // check if pthread_create
  if (resPthreadCreate != 0) {
    free(plcProxy);
    return NULL;
  }

  return plcProxy;
}

/**
 * @brief Gracefully terminate a PLC Proxy instance
 * @param[in] plc Proxy instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endPlcProxy(PlcProxy_t* plc) {
  if (plc == NULL) {
    return -1;
  }

  plc->finished = true;

  int retVal = 0;
  (void)pthread_join(plc->readerThreadTid, (void*)&retVal);
  // check if the join failed
  if (retVal != 0) {
    return -1;
  }
  sem_destroy(&(plc->mutex));

  // closing all open pipes
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    if (plcProxy->outputFd[i][0] == -1) {
      continue;
    }
    close(plcProxy->outputFd[i][0]);
    close(plcProxy->outputFd[i][1]);
  }

  free(plc);
  return 0;
}

/**
 * @brief Send a message to the PLC
 * @param[in] plc Proxy instance handle
 * @param[in] msg Pointer to the message to send
 * @return 0 on success, non-zero error code on failure
 */
int sendMessagePlcProxy(PlcProxy_t* plc, PlcMessage_t* msg) {
  sem_wait(&(plc->mutex));

  // TODO(felipe): send message plc
  sleep(10);

  sem_post(&(plc->mutex));
  return 0;
}

/**
 * @brief Read a message from the PLC
 * @param[in] plc Proxy instance handle
 * @return Pointer to the received message, or NULL on failure
 */
PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc) {
  // TODO(felipe): read a message from the plc & format it into object
  sleep(10);
  return 0;
}

/**
 * @brief Entry point for the PLC message receiver thread
 * @param[in] plcProxy Pointer to the PlcProxy_t instance
 * @return Thread exit status (always NULL)
 */
void* plcProxyMsgReceiverThread(void* plcProxy) {
  PlcProxy_t* plcProxy = (PlcProxy_t*)plcProxy;
  while (!plcProxy->finished) {
    PlcMessage_t* msg = readMessagePlcProxy();
    // TODO(andre):check the target is registered
    // copy message to correct output file descriptor
  }
}

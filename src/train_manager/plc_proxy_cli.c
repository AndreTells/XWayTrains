#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "plc/plc_info.h"
#include "plc/plc_proxy.h"

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
 * @brief Attempt to register a client with the plc proxy
 * @param[in] resManager Proxy instance handle
 * @param[in] clientId ID of the client to register
 * @return 0 on success, non-zero error code on failure
 */
int plcProxyTryRegisterClient(PlcProxy_t* plcProxy, int clientId);

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
  pthread_kill(plc->readerThreadTid, SIGINT);
  (void)pthread_join(plc->readerThreadTid, (void*)&retVal);
  // check if the join failed
  if (retVal != 0) {
    return -1;
  }
  sem_destroy(&(plc->mutex));

  // closing all open pipes
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    if (plc->outputFd[i][0] == -1) {
      continue;
    }
    close(plc->outputFd[i][0]);
    close(plc->outputFd[i][1]);
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
  printf("msg %x sent to plc\n", msg);
  sem_post(&(plc->mutex));
  return 0;
}

/**
 * @brief Read a message from the PLC
 * @param[in] plc Proxy instance handle
 * @param[in] clientId id of who is attempting to read as message
 * @return Pointer to the received message, or NULL on failure
 */
PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, int clientId) {
  if (plcProxyTryRegisterClient(plc, clientId) != 0) {
    return NULL;
  }

  size_t msgSize = getMessageSize(getNullMessage());
  PlcMessage_t* res = (PlcMessage_t*)malloc(msgSize);
  // considers all messages have the same size for testing purposes
  ssize_t res_size = read(plc->outputFd[clientId][0], res, msgSize);
  if (res_size == 0) {
    return NULL;
  }

  printf("%d received %x with size %d from %d \n", clientId, res, res_size,
         plc->outputFd[clientId][0]);
  return res;
}

/**
 * @brief Entry point for the PLC message receiver thread
 * @param[in] plcProxy Pointer to the PlcProxy_t instance
 * @return Thread exit status (always NULL)
 */
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

/**
 * @brief Attempt to register a client with the plc proxy
 * @param[in] resManager Proxy instance handle
 * @param[in] clientId ID of the client to register
 * @return 0 on success, non-zero error code on failure
 */
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

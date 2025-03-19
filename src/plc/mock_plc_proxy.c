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

  plcProxy->finished = false;

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
  return 0;
}

/**
 * @brief Read a message from the PLC
 * @param[in] plc Proxy instance handle
 * @param[in] clientId id of who is attempting to read as message
 * @return Pointer to the received message, or NULL on failure
 */
PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, int clientId) {
  return getNullMessage();
}

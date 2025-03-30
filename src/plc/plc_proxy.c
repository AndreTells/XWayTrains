#include "plc/plc_proxy.h"

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common/comm_general.h"
#include "common/time_out.h"
#include "common/verbose.h"
#include "plc/model_info.h"
#include "plc/plc_message.h"

#define MAX_NUM_REGISTRABLE_TRAINS 5  // ignore position 0

#define XWAY_NETWORK 1
#define XWAY_PORT 0

struct PlcProxy_t {
  pthread_t readerThreadTid;
  sem_t mutex;
  int outputFd[MAX_NUM_REGISTRABLE_TRAINS][2];
  bool finished;
  uint8_t remoteStation;
  int sock_fd;
};

PlcMessage_t* tryGetPlcMessage(int fd);

void print_data_hex(const uint8_t* data);

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
int plcProxyTryRegisterClient(PlcProxy_t* plcProxy, enum TrainId_e clientId);

PlcProxy_t* initPlcProxy(char* plcIpAddr, const uint16_t port, uint8_t remoteStation) {
  // check if it's a valid IP address
  verbose("[PLC PROXY]: Initializing ... \n");
  if (plcIpAddr == NULL) {
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
    return NULL;
  }

  PlcProxy_t* plcProxy = (PlcProxy_t*)malloc(sizeof(PlcProxy_t));

  if (plcProxy == NULL) {
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
    return NULL;
  }

  plcProxy->sock_fd = tcpCreateSocketWrapper(false, NULL, port);

  if (plcProxy->sock_fd == -1) {
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
    return NULL;
  }

  int connRes = tcpConnectWrapper(plcProxy->sock_fd, plcIpAddr, port);

  if (connRes == -1) {
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
    return NULL;
  }

  plcProxy->finished = false;
  plcProxy->remoteStation = remoteStation;

  // Initialize the output file descriptors to -1 (invalid)
  for (int i = 0; i < MAX_NUM_REGISTRABLE_TRAINS; i++) {
    plcProxy->outputFd[i][0] = -1;
    plcProxy->outputFd[i][1] = -1;
  }

  int resSemInit = sem_init(&(plcProxy->mutex), 0, 1);
  // check if sem_init failed
  if (resSemInit != 0) {
    free(plcProxy);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
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
    verbose("[PLC PROXY]: Initializing Reader Thread ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
    verbose("[PLC PROXY]: Initializing ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
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

  verbose("[PLC PROXY]: Ending Reader Thread ... " VERBOSE_KGRN
          "success \n" VERBOSE_RESET);
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

  verbose("[PLC PROXY]: Closing Pipes ... " VERBOSE_KGRN
          "success \n" VERBOSE_RESET);

  free(plc);
  return 0;
}

ssize_t sendMessagePlcProxy(PlcProxy_t* plc, PlcMessage_t* msg, uint8_t hostStation) {
  verbose("[PLC PROXY]:attempting to send message to plc\n");
  // configuring the networking aspect

  XwayAddr hostXwayAddr = createXwayAddr(hostStation, XWAY_NETWORK, XWAY_PORT);
  XwayAddr remoteXwayAddr = createXwayAddr(plc->remoteStation, XWAY_NETWORK, XWAY_PORT);

  uint8_t extAddr[2] = {0,0};
  extAddr[0] = (uint8_t)SEND_CODE;
  extAddr[1] = 0x10;
  int res = setNPDU(msg, NPDU_5WAY, hostXwayAddr, remoteXwayAddr,
                   extAddr);

  if (res < 0) {
    return res;
  }

  sem_wait(&(plc->mutex));

  int sendRes = sendPlcMessageToFd(msg, plc->sock_fd);

  if (sendRes < 0) {
    verbose("failed with errno %d \n", errno);
  }

  sem_post(&(plc->mutex));

  return sendRes;
}

PlcMessage_t* readMessagePlcProxy(PlcProxy_t* plc, enum TrainId_e clientId) {
  if (plcProxyTryRegisterClient(plc, clientId) != 0) {
    return NULL;
  }
  PlcMessage_t* msg;
  while (!plc->finished) {
    msg = tryGetPlcMessage(plc->outputFd[clientId][0]);
    if (msg != NULL) {
      break;
    }
  }

  if (msg == NULL) {
    return msg;
  }

  verbose("[PLC PROXY]: sending read ack to plc\n");

  PlcMessage_t* ack = createACK(msg, true);

  sem_wait(&(plc->mutex));

  int sendRes = sendPlcMessageToFd(ack, plc->sock_fd);

  if (sendRes < 0) {
    verbose("failed with errno %d \n", errno);
  }

  sem_post(&(plc->mutex));

  free(ack);

  if (sendRes < -1) {
    free(msg);
    return NULL;
  }

  return msg;
}

void* plcProxyMsgReceiverThread(void* plcProxy) {
  PlcProxy_t* plc = (PlcProxy_t*)plcProxy;

  verbose("[PLC PROXY]: Receiver thread initialized \n");
  while (!plc->finished) {
    PlcMessage_t* msg = tryGetPlcMessage(plc->sock_fd);

    // if failed to get a msg, retry
    if (msg == NULL) {
      continue;
    }

    verbose("[PLC PROXY]: read a line \n");

    // things that aren't write
    if (!compareMsgType(msg, APDU_WRITE_REQ)) {
      free(msg);
      continue;
    }

    verbose("[PLC PROXY]: the message is a write \n");

    // determining who to route the message to
    uint8_t* data = getPlcMessageData(msg);
    TrainId_e target = getTargetedTrain(data);

    if (plcProxyTryRegisterClient(plc, target) == 0) {
      verbose("[PLC PROXY]: rerouted a message \n");
      sendPlcMessageToFd(msg, plc->outputFd[target][1]);
    }

    free(msg);
  }

  pthread_exit(NULL);
}

int plcProxyTryRegisterClient(PlcProxy_t* plcProxy, enum TrainId_e clientId) {
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

PlcMessage_t* tryGetPlcMessage(int fd) {
  uint8_t serMsg[MAX_MSG_SIZE];

  int resWait = fileDescriptorTimedWait(fd);
  if (resWait < 0) {
    return NULL;
  }

  // Read enough to get the message size
  ssize_t bytesRead = 0;
  while (bytesRead < MSG_SIZE_POS) {
    ssize_t res = read(fd, serMsg + bytesRead, MSG_SIZE_POS - bytesRead);
    if (res == -1) {
      perror("read error");
      return NULL;
    } else if (res == 0) {
      // Connection closed
      return NULL;
    }
    bytesRead += res;
  }

  uint8_t msgSize = serMsg[MSG_SIZE_POS - 1];

  // Validate msgSize to prevent buffer overflow
  if (msgSize > MAX_MSG_SIZE - MSG_SIZE_POS) {
    fprintf(stderr, "Invalid message size: %u\n", msgSize);
    return NULL;
  }

  // Read the rest of the message
  bytesRead = 0;
  while (bytesRead < msgSize) {
    ssize_t res = read(fd, serMsg + MSG_SIZE_POS + bytesRead, msgSize - bytesRead);
    if (res == -1) {
      perror("read error");
      return NULL;
    } else if (res == 0) {
      return NULL;
    }
    bytesRead += res;
  }

  print_data_hex(serMsg);

  PlcMessage_t* msg = deserializePlcMessage(serMsg);

  return msg;
}

// does not consider the situation where it failed to connect to a server
int sendPlcMessageToFd(PlcMessage_t* msg,  int fd) {
  uint8_t serMsg[MAX_MSG_SIZE];
  memset(serMsg, 0, MAX_MSG_SIZE);

  size_t serSize = serializePlcMessage(msg, serMsg);
  print_data_hex(serMsg);

  int writeRes = (int)write(fd, serMsg, serSize);

  return writeRes;
}

void print_data_hex(const uint8_t* data) {
  verbose("\tData (HEX): ");
  int len = data[5] + 6;
  for (int i = 0; i < len; i++) {
    if (i == 5 || i == 6 || i == 8 || i == 18) verbose("| ");
    verbose("%02X ", data[i]);
  }
  verbose("\n");
  verbose(
      "\tPosition:    0  1  2  3  4 |  5 |  6  7 |  8  9 10 11 12 13 14 15 16 "
      "17 | 18 "
      "19 20 21 22 23 24 25 26 27\n\n");
}

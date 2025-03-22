#include "train.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "comm.h"
#include "model_info.h"
#include "plc_info.h"
#include "plc_proxy.h"
#include "resource_manager_proxy.h"

#define MAX_BUFFER_LEN 256

/**
 * @brief Structure representing a Train instance
 * @details Encapsulates all train-related state and functionality.
 */
struct Train_t {
  pthread_t trainTid;
  enum TrainId_e trainId;
  PlcProxy_t* plc;
  ResourceManagerProxy_t* resManager;
  bool finished;
  char* routeFilePath;
};

/**
 * @brief Entry point for the train thread
 * @param[in] data Pointer to the Train_t instance
 * @return Thread exit status (always NULL)
 */
void* trainThread(void* data);

/**
 * @brief Initialize a new Train instance
 * @param[in] trainId Identifier for the train (from the TrainId_e enum)
 * @param[in] plc Proxy instance for communication with the PLC
 * @param[in] resManager Proxy instance for communication with the Resource
 * Manager
 * @return Pointer to the newly created Train_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endTrain()`.
 */
Train_t* initTrain(enum TrainId_e trainId, PlcProxy_t* plc,
                   ResourceManagerProxy_t* resManager, char* routeFilePath) {
  // accounting for invalid inputs
  if (plc == NULL || resManager == NULL) {
    return NULL;
  }

  Train_t* train = (Train_t*)malloc(sizeof(Train_t));
  // check if malloc failed
  if (train == NULL) {
    return train;
  }

  // configuring the object with the given parameters
  train->trainId = trainId;
  train->plc = plc;
  train->resManager = resManager;
  train->finished = false;
  train->routeFilePath = routeFilePath;

  // creating train thread
  pthread_create(&(train->trainTid), NULL, trainThread, (void*)train);

  return train;
}

/**
 * @brief Gracefully terminate a Train instance
 * @param[in] train Train instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endTrain(Train_t* train) {
  train->finished = true;
  int retVal = 0;
  (void)pthread_join(train->trainTid, (void*)&retVal);
  // check if the join failed
  if (retVal != 0) {
    return -1;
  }
  free(train);
  return 0;
}

/**
 * @brief Entry point for the train thread
 * @param[in] train Pointer to the Train_t instance
 * @return Thread exit status (always NULL)
 */
void* trainThread(void* data) {
  Train_t* train = (Train_t*)data;
  // open route file
  const char* path = "./data/train1.csv";
  if (access(path, F_OK) == 0) {
    perror("File does not exist");
    exit(EXIT_FAILURE);
  }

  FILE* route = fopen(path, "r+");
  char buffer[MAX_BUFFER_LEN];
  memset(buffer, 0, MAX_BUFFER_LEN);
  fgets(NULL, 0, route);  // skip header

  word_t section_id = UNCHANGED;
  word_t switch_id = UNCHANGED;
  word_t actuator = UNCHANGED;

  while (!train->finished) {
    // read next line of route file
    while (fgets(buffer, MAX_BUFFER_LEN, route) != NULL) {
      char* token = strtok(buffer, "\t");

      // TODO USE str_to_uint16
      actuator = atoi(token);

      token = strtok(NULL, "\t");
      if (strncmp(token, "Section", 10) == 0) {
        section_id = actuator;
      } else if (strncmp(token, "Switch", 10) == 0) {
        switch_id = actuator;
      } else if (strncmp(token, "Inversion", 10) == 0) {
        section_id = actuator + 40;

      } else {
      }
    }

    // interpret line
    // interpret message
    // generate appropriate message
    PlcMessage_t* cmd_msg = getNullMessage();
    sendMessagePlcProxy(train->plc, cmd_msg);
    PlcMessage_t* resp_msg = readMessagePlcProxy(train->plc, train->trainId);
    // check if there was an error
    sleep(5);
  }

  pthread_exit(NULL);
}

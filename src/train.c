#include "train.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "model_info.h"
#include "plc_proxy.h"
#include "resource_manager_proxy.h"

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
  pthread_create(&(train->trainTid), NULL, (void* (*)(void*))trainThread,
                 (void*)train);

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
void* trainThread(Train_t* train) {
  // open route file
  while (!train->finished) {
    // TODO(andre): do the following
    // get a line from route file
    // send it to interpreter
    // check if file is done
    // if yes, set train->finished = true;
    sleep(10);
  }

  pthread_exit(NULL);
}

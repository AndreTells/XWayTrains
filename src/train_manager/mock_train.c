#include "train_manager/train.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "plc/model_info.h"
#include "plc/plc_message.h"
#include "plc/plc_proxy.h"
#include "train_manager/resource_manager_proxy.h"

struct Train_t {
  enum TrainId_e trainId;
  PlcProxy_t* plc;
  ResourceManagerProxy_t* resManager;
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
Train_t* initTrain(PlcProxy_t* plc, ResourceManagerProxy_t* resManager,
                   char* routeFilePath) {
  // accounting for invalid inputs
  if (plc == NULL || resManager == NULL) {
    return NULL;
  }

  Train_t* train = (Train_t*)malloc(sizeof(Train_t)); // check if malloc failed if (train == NULL) { return train; }

  train->trainId = UNKNOWN_TRAIN;
  train->plc = plc;
  train->resManager = resManager;

  return train;
}

/**
 * @brief Gracefully terminate a Train instance
 * @param[in] train Train instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endTrain(Train_t* train) {
  free(train);
  return 0;
}

int setTrainId(Train_t* train, int id){
  if(id > TRAIN_4 || id == UNKNOWN_TRAIN){
    return -1;
  }
  train->trainId = (TrainId_e) id;
  return 0;
}

int getTrainId(Train_t* train){
  return train->trainId;
}

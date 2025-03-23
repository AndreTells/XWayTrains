#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "plc/model_info.h"
#include "plc/plc_proxy.h"
#include "train_manager/resource_manager_proxy.h"
#include "train_manager/train.h"

struct Train_t {
  enum TrainId_e trainId;
  PlcProxy_t* plc;
  ResourceManagerProxy_t* resManager;
};

Train_t* initTrain(PlcProxy_t* plc, ResourceManagerProxy_t* resManager,
                   [[maybe_unused]] char* routeFilePath) {
  // accounting for invalid inputs
  if (plc == NULL || resManager == NULL) {
    return NULL;
  }

  Train_t* train = (Train_t*)malloc(sizeof(
      Train_t));  // check if malloc failed if (train == NULL) { return train; }

  train->trainId = UNKNOWN_TRAIN;
  train->plc = plc;
  train->resManager = resManager;

  return train;
}

int endTrain(Train_t* train) {
  free(train);
  return 0;
}

int setTrainId(Train_t* train, int id) {
  if (id > TRAIN_4 || id == UNKNOWN_TRAIN) {
    return -1;
  }
  train->trainId = (TrainId_e)id;
  return 0;
}

enum TrainId_e getTrainId(Train_t* train) { return train->trainId; }

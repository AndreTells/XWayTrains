#include "train_manager/train.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "common/verbose.h"
#include "plc/model_info.h"
#include "plc/plc_proxy.h"
#include "train_manager/interpreter.h"
#include "train_manager/resource_manager_proxy.h"

Train_t* initTrain(PlcProxy_t* plc, ResourceManagerProxy_t* resManager,
                   char* routeFilePath, uint8_t xwayStation) {
  verbose("[Train]: initialisation ... \n");
  // accounting for invalid inputs
  if (plc == NULL || resManager == NULL) {
    return NULL;
  }

  Train_t* train = (Train_t*)malloc(sizeof(
      Train_t));  // check if malloc failed if (train == NULL) { return train; }

  train->trainId = UNKNOWN_TRAIN;
  train->plc = plc;
  train->resManager = resManager;
  train->xwayStation = xwayStation;

  verbose("[Train]: Opening route file ... \n");
  train->path = initPath(routeFilePath);
  if (train->path == NULL) {
    verbose("[Train]: Opening route file ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);

    free(train);
    return NULL;
  }
  verbose("[Train]: Opening route file ... " VERBOSE_KGRN
          "success \n" VERBOSE_RESET);

  verbose("[Train]: initialisation ... " VERBOSE_KGRN
          "success \n" VERBOSE_RESET);
  return train;
}

int endTrain(Train_t* train) {
  fclose(train->path);
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

int executeRoute(Train_t* train) {
  verbose("[Train]: Executing Train route ... \n");
  rewind(train->path);

  // read a line
  while (true) {
    verbose("[Train]: reading a new line ... \n");
    char* cmd = readPathLine(train->path);
    if (cmd == NULL) {
      break;
    }

    verbose("[Train]: Executing command ... \n");
    int execRes =
        executeCommand(cmd, train, train->plc,train->resManager);
    if (execRes != 0) {
      verbose("[Train]: Executing command ... \n" VERBOSE_KRED
              "fail \n" VERBOSE_RESET);

      return -1;
    }


    free(cmd);
  }

  return 0;
}

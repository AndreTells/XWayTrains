#include <stdio.h>

#include "model_info.h"
#include "plc_proxy.h"
#include "resource_manager_proxy.h"
#include "train.h"

int main(void) {
  ResourceManagerProxy_t* resMan = initResourceManagerProxy(" ");
  if (resMan == NULL) {
    printf("failed to create resource manager\n");
    return -1;
  }

  PlcProxy_t* plc = initPlcProxy(" ");
  if (plc == NULL) {
    printf("failed to create plc proxy\n");
    return -1;
  }
  Train_t* train = initTrain(TRAIN_1, plc, resMan, " ");

  (void)endResourceManagerProxy(resMan);
  (void)endPlcProxy(plc);
  (void)endTrain(train);
  return 0;
}

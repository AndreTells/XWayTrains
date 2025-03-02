#include <stdio.h>
#include "plc_proxy.h"

int main(void){
  PlcProxy_t* plc = initPlcProxy(" ");
  if(plc == NULL){
    printf("failed to create resource manager\n");
    return -1;
  }

  (void)endPlcProxy(plc);
  return 0;
}

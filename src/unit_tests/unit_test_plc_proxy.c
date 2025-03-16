#include <stdio.h>

#include "plc/plc_info.h"
#include "plc/plc_proxy.h"

int main(void) {
  PlcProxy_t* plc = initPlcProxy(" ");
  if (plc == NULL) {
    printf("failed to create plc proxy\n");
    return -1;
  }

  sendMessagePlcProxy(plc, getNullMessage());
  (void)readMessagePlcProxy(plc, 0);

  (void)endPlcProxy(plc);
  return 0;
}

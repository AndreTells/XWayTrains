#include <stdio.h>

#include "resource_manager_proxy.h"

int main(void) {
  ResourceManagerProxy_t* resMan = initResourceManagerProxy(" ");
  if (resMan == NULL) {
    printf("failed to create resource manager\n");
    return -1;
  }
  int res = 0;
  res = requestResource(resMan, 3, 0);
  printf("response to 0 request: %d\n", res);

  res = releaseResource(resMan, 3, 0);
  printf("response to 0 release: %d\n", res);

  res = requestResource(resMan, 3, 3);
  printf("response to 3 request: %d\n", res);

  res = releaseResource(resMan, 3, 3);
  printf("response to 3 release: %d\n", res);

  (void)endResourceManagerProxy(resMan);
  return 0;
}

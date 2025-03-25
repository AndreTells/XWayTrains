#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common/comm_general.h"
#include "common/flags.h"
#include "common/resource_request.h"
#include "common/verbose.h"
#include "train_manager/resource_manager_proxy.h"

ResourceManagerProxy_t* resManager = NULL;

void handle_sigint(int sig) {
  if (sig != SIGINT) {
    return;
  }
  verbose(
      "\n[RESOURCE MANAGER TEST CLIENT]: Ctr-C Captured. Exiting Program \n");

  if (resManager != NULL) {
    (void)endResourceManagerProxy(resManager);
  }

  exit(0);
}

int main(int argc, char* argv[]) {
  // setting up signal handling
  signal(SIGINT, handle_sigint);

  verbose(
      "[RESOURCE MANAGER TEST CLIENT]: Utilize Ctr-C to end this program\n");

  // checking command line arguments
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  char* resource_s = "0";
  (void)get_flag_value(argc, argv, "--resource", &resource_s);
  int resource = atoi(resource_s);

  verbose("[RESOURCE MANAGER TEST CLIENT]: Creating the socket\n");
  char* ip_address;
  bool ip_received = get_flag_value(argc, argv, IP_ADDRESS_FLAG, &ip_address);

  if (!ip_received) {
    verbose("[RESOURCE MANAGER TEST CLIENT]: ERROR No Ip Address Received\n");
    return -1;
  }

  resManager = initResourceManagerProxy(ip_address, 8080);

  assert(resManager != NULL);

  verbose("[RESOURCE MANAGER TEST CLIENT]: Requesting Ressource\n");

  int reqResp = requestResource(resManager, LOCK_RESOURCE, (uint8_t)resource, 0);
  assert(reqResp == 0);

  verbose("[RESOURCE MANAGER TEST CLIENT]: Request Granted\n");
  endResourceManagerProxy(resManager);
  return 0;
}

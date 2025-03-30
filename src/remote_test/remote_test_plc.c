#include <assert.h>
#include <signal.h>
#include <stdint.h>

#include "common/flags.h"
#include "common/verbose.h"
#include "plc/plc_facade.h"
#include "plc/plc_message.h"
#include "plc/plc_proxy.h"

#define HOST_IP "172.31.71.25"

#define PLC_REMOTE_IP "10.31.125.14"
#define PLC_PORT 502
#define XWAY_HOST_STATION 0x28
#define XWAY_REMOTE_STATION 0x0E
#define XWAY_NETWORK 1
#define XWAY_PORT 0

PlcProxy_t* plc = NULL;

void handle_sigint(int sig) {
  if (sig != SIGINT) {
    return;
  }
  verbose("\n[PLC TEST CLIENT]: Ctr-C Captured. Exiting Program \n");

  if (plc != NULL) {
    (void)endPlcProxy(plc);
  }

  exit(0);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, handle_sigint);

  // checking for flags
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  char* train_s = "1";
  (void)get_flag_value(argc, argv, "--train", &train_s);
  int train = atoi(train_s);
  verbose("[PLC TEST CLIENT]: targeting train %d \n", train);

  char* target_s = "3";
  (void)get_flag_value(argc, argv, "--target", &target_s);
  int target = atoi(target_s);
  verbose("[PLC TEST CLIENT]: targeting switich %d \n", target);

  // sending out messages
  plc = initPlcProxy(PLC_REMOTE_IP, PLC_PORT, XWAY_REMOTE_STATION);

  assert(plc != NULL);

  /* making a message */
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  int res;

  verbose("[PLC TEST CLIENT]: Configuring Message ... \n");
  res = configWritePlcMessage(msg, TOGGLE_RAIL, XWAY_HOST_STATION,
                              (TrainId_e)train, (uint16_t)target);
  assert(res == 0);

  // attempting to send the message
  verbose("[PLC TEST CLIENT]: attempting to send message ... \n");
  ssize_t msgSize = sendMessagePlcProxy(plc, msg, XWAY_HOST_STATION);
  assert(0 < msgSize);
  verbose("[PLC TEST CLIENT]: attempting to send message ... " VERBOSE_KGRN
          "success \n" VERBOSE_RESET);

  verbose("[PLC TEST CLIENT]: attempting to read response ...\n");
  PlcMessage_t* receivedMsg = readMessagePlcProxy(plc, TRAIN_1);
  verbose("[PLC TEST CLIENT]: attempting to read response\n" VERBOSE_KGRN
          "success \n" VERBOSE_RESET);

  assert(receivedMsg != NULL);

  free(receivedMsg);

  (void)endPlcProxy(plc);
  return 0;
}

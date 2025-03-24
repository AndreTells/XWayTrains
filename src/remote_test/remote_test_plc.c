#include <signal.h>
#include <stdint.h>
#include <assert.h>
#include "plc/plc_message.h"
#include "plc/plc_facade.h"
#include "plc/plc_proxy.h"
#include "common/flags.h"
#include "common/verbose.h"

#define HOST_ADDR "172.31.71.25"
#define SERVER_ADDR "10.31.125.14"
#define PLC_PORT 502

PlcProxy_t* plc = NULL;

void handle_sigint(int sig) {
  if(sig != SIGINT){
    return;
  }
  verbose("\n[PLC TEST CLIENT]: Ctr-C Captured. Exiting Program \n");

  if(plc != NULL){
    (void)endPlcProxy(plc);
  }

  exit(0);
}

int main(int argc, char* argv[]){

  signal(SIGINT, handle_sigint);

  // checking for flags
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  char* train_s = "1";
  (void)get_flag_value(argc,argv,"--train",&train_s);
  int train = atoi(train_s);
  verbose("[PLC TEST CLIENT]: targeting train %d \n", train);

  char* target_s = "20";
  (void)get_flag_value(argc,argv,"--target",&train_s);
  int target = atoi(target_s);
  verbose("[PLC TEST CLIENT]: targeting rail %d \n", train);

  // sending out messages
  plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT);

  uint8_t pc_station = 0x28;
  uint8_t plc_station = 0x0E;

  uint8_t network = 1;
  uint8_t port = 0;
  int netRes = setXwayAddrs(plc ,  pc_station, plc_station, network, port);

  assert(plc != NULL);
  assert(netRes == 0);

  /* making a message */
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  int res;

   res = configWritePlcMessage(msg, TOGGLE_RAIL, pc_station , train, target);
  assert(res == 0);

  // attempting to send the message
  verbose("[PLC TEST CLIENT]: attempting to send message ... \n");
  int ret = sendMessagePlcProxy(plc, msg);
  assert(ret == 0);
  verbose("[PLC TEST CLIENT]: attempting to send message ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[PLC TEST CLIENT]: attempting to read response ...\n");
  PlcMessage_t* receivedMsg = readMessagePlcProxy(plc, TRAIN_1);
  verbose("[PLC TEST CLIENT]: attempting to read response\n"VERBOSE_KGRN "success \n" VERBOSE_RESET);
  assert(receivedMsg != NULL);
  assert(compareMsgType(receivedMsg, APDU_WRITE_REQ) == 0);

  free(receivedMsg);
  return 0;
}

#include <signal.h>
#include <stdint.h>
#include <assert.h>
#include "plc/plc_message.h"
#include "plc/plc_facade.h"
#include "plc/plc_proxy.h"
#include "common/flags.h"
#include "common/verbose.h"

#define HOST_ADDR "127.0.0.1"
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

  char* target_s = "20";
  (void)get_flag_value(argc,argv,"--target",&train_s);
  int target = atoi(target_s);

  // sending out messages
  PlcProxy_t* proxy = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT);
  assert(proxy != NULL);

  /* making a message */
  PlcMessage_t* msg = createPlcMessage();
  assert(msg != NULL);
  int res;
  uint8_t pc_station = 0x28;
  uint8_t plc_station = 0x0E;

  uint8_t network = 1;
  uint8_t port = 0;

   res = configWritePlcMessage(msg, TOGGLE_SWITCH, pc_station , TRAIN_1, SWITCH_GROUP_31);
  assert(res == 0);

  /* configure the address information */
  XwayAddr sender = createXwayAddr(pc_station, network, port);
  XwayAddr receiver = createXwayAddr(plc_station, network, port);
  uint8_t extAddr[2] = {0x09, 0x10};
  res = setNPDU(msg, NPDU_5WAY, sender, receiver, extAddr);
  assert(res == 0);

  // attempting to send the message
  verbose("[PLC TEST CLIENT]: attempting to send message ... \n");
  int ret = sendMessagePlcProxy(proxy, msg);
  assert(ret == 0);
  verbose("[PLC TEST CLIENT]: attempting to send message ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  verbose("[PLC TEST CLIENT]: attempting to read response ...\n");
  PlcMessage_t* receivedMsg = readMessagePlcProxy(proxy, TRAIN_1);
  verbose("[PLC TEST CLIENT]: attempting to read response\n"VERBOSE_KGRN "success \n" VERBOSE_RESET);
  assert(receivedMsg != NULL);
  assert(compareMsgType(receivedMsg, APDU_WRITE_REQ) == 0);

  free(receivedMsg);
  return 0;
}

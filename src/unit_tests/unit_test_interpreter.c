#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "plc/plc_message.h"
#include "plc/plc_facade.h"
#include "train_manager/resource_manager_proxy.h"
#include "train_manager/interpreter.h"
#include "train_manager/train.h"
#include "common/verbose.h"
#include "common/flags.h"

#define HOST_ADDR "127.0.0.1"
#define SERVER_ADDR "10.31.125.14"
#define PLC_PORT 502

const uint8_t station = 0x28;

void test_executeCommand_nullArguments() {
  verbose("[Interpreter] executeCommand NULL Args ... \n");

  char cmd[] = "trainId 5";

  /* Allocate non-NULL pointers using malloc; actual implementations are provided by mocks. */
  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ", 0);
  Train_t* train = initTrain(plc,resMgr," ");

  assert(executeCommand(NULL, train, plc, station, resMgr) == -1);
  assert(executeCommand(cmd, NULL, plc,station, resMgr) == -1);
  assert(executeCommand(cmd, train, NULL,station, resMgr) == -1);
  assert(executeCommand(cmd, train, plc,station, NULL) == -1);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand NULL Args ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_executeCommand_setTrainId_success() {
  verbose("[Interpreter] executeCommand setTrainId ... \n");

  char cmd[] = "trainId 4";

  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ", 0);
  Train_t* train = initTrain(plc,resMgr," ");

  int ret = executeCommand(cmd, train, plc, station,resMgr);
  assert(ret == 0);

  /* Optionally, if your mock updates the train state, verify train id was set to 7 */

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand setTrainId ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_executeCommand_plc_invalidParams() {
  verbose("[Interpreter] executeCommand plc Invalid Params ... \n");

  char cmd[] = "plc rail"; // Missing target id parameter

  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ", 0);
  Train_t* train = initTrain(plc,resMgr," ");

  int ret = executeCommand(cmd, train, plc,station, resMgr);
  assert(ret == -1);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand plc Invalid Params ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_executeCommand_plc_validParams() {
  verbose("[Interpreter] executeCommand plc Valid Params ... \n");

  int ret = 0;

  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ", 0);
  Train_t* train = initTrain(plc,resMgr," ");

  // setVerbose(true);
  char cmd [20] = "plc rail 22";
  ret = executeCommand(cmd, train, plc,station, resMgr);
  assert(ret == 0);

  strcpy(cmd,"plc switch 7");
  ret = executeCommand(cmd, train, plc,station, resMgr);
  assert(ret == 0);

  strcpy(cmd, "plc invert 4");
  ret = executeCommand(cmd, train, plc,station, resMgr);
  assert(ret == 0);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand plc Invalid Params ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_executeCommand_resource_invalidParams() {
  verbose("[Interpreter] executeCommand resource Invalid Params ... \n");

  char cmd[] = "resource"; // Missing request type and resource IDs

  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ", 0);
  Train_t* train = initTrain(plc,resMgr," ");

  int ret = executeCommand(cmd, train, plc,station, resMgr);
  assert(ret == -1);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand resource Invalid Params ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_initPath_destroyInterpreter() {
  verbose("[Interpreter] initPath & destroyInterpreter ... \n");

  const char* tempFile = "data/train1.csv";
  // Write sample data to file
  FILE* fp = fopen(tempFile, "w");
  assert(fp != NULL);
  fputs("Train Id	1\n"
    "Id	Type	Resources\n"
    "3	rail	1\n"
    "3	switch\n"
    "10	invert\n", fp);
  fclose(fp);

  // Open it
  Path_t path = initPath(tempFile);
  assert(path != NULL);

  // Read it
  char* line = readPathLine(path);
  assert(line != NULL);
  assert(strcmp(line, "Train Id	1\n") == 0);
  free(line);

  int ret = destroyInterpreter(path);
  assert(ret == 0);

  assert(remove(tempFile) == 0);

  verbose("[Interpreter] initPath & destroyInterpreter ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Interpreter ... \n\n");

  test_executeCommand_nullArguments();
  test_executeCommand_setTrainId_success();
  test_executeCommand_plc_invalidParams();
  test_executeCommand_plc_validParams();
  test_executeCommand_resource_invalidParams();
  test_initPath_destroyInterpreter();

  verbose("\n[Unit Testing] Interpreter ... Done \n");
  return 0;
}

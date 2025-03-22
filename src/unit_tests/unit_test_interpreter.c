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


void test_executeCommand_nullArguments() {
  verbose("[Interpreter] executeCommand NULL Args ... \n");

  char cmd[] = "trainId 5";

  /* Allocate non-NULL pointers using malloc; actual implementations are provided by mocks. */
  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ");
  Train_t* train = initTrain(plc,resMgr," ");

  assert(executeCommand(NULL, train, plc, resMgr) == -1);
  assert(executeCommand(cmd, NULL, plc, resMgr) == -1);
  assert(executeCommand(cmd, train, NULL, resMgr) == -1);
  assert(executeCommand(cmd, train, plc, NULL) == -1);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand NULL Args ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_executeCommand_setTrainId_success() {
  verbose("[Interpreter] executeCommand setTrainId ... \n");

  char cmd[] = "trainId 4";

  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ");
  Train_t* train = initTrain(plc,resMgr," ");

  int ret = executeCommand(cmd, train, plc, resMgr);
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
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ");
  Train_t* train = initTrain(plc,resMgr," ");

  int ret = executeCommand(cmd, train, plc, resMgr);
  assert(ret == -1);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand plc Invalid Params ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_executeCommand_resource_invalidParams() {
  verbose("[Interpreter] executeCommand resource Invalid Params ... \n");

  char cmd[] = "resource"; // Missing request type and resource IDs

  PlcProxy_t* plc = initPlcProxy(HOST_ADDR, SERVER_ADDR, PLC_PORT );
  ResourceManagerProxy_t* resMgr = initResourceManagerProxy(" ");
  Train_t* train = initTrain(plc,resMgr," ");

  int ret = executeCommand(cmd, train, plc, resMgr);
  assert(ret == -1);

  endTrain(train);
  endResourceManagerProxy(resMgr);
  endPlcProxy(plc);

  verbose("[Interpreter] executeCommand resource Invalid Params ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_initPath_destroyInterpreter() {
  verbose("[Interpreter] initPath & destroyInterpreter ... \n");

  const char* tempFile = "temp_test_file.txt";
  FILE* fp = fopen(tempFile, "w");
  assert(fp != NULL);
  fputs("test\n", fp);
  fclose(fp);

  Path_t path = initPath((char*)tempFile);
  assert(path != NULL);

  char* line = readPathLine(path);
  assert(line != NULL);
  assert(strcmp(line, "test\n") == 0);
  free(line);

  int ret = destroyInterpreter(path);
  assert(ret == 0);

  remove(tempFile);

  verbose("[Interpreter] initPath & destroyInterpreter ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc, argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Interpreter ... \n\n");

  test_executeCommand_nullArguments();
  test_executeCommand_setTrainId_success();
  test_executeCommand_plc_invalidParams();
  test_executeCommand_resource_invalidParams();
  test_initPath_destroyInterpreter();

  verbose("\n[Unit Testing] Interpreter ... Done \n");
  return 0;
}

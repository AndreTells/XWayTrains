#include "train_manager/interpreter.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/comm_general.h"
#include "common/verbose.h"
#include "plc/plc_facade.h"
#include "plc/plc_message.h"
#include "train_manager/train.h"

#define MAX_LINE_SIZE 50
#define MAX_RESOURCE_REQUEST_AMM 10

static const KeywordToken interpreterCommandTable[] = {
    {"trainId", CMD_SET_TRAIN_ID},
    {"plc", CMD_SET},
    {"resource", CMD_RESOURCE}};
static const int interpreterCommandTableSize = 3;

static const KeywordToken plcMsgTypeTable[] = {
    {"rail", TOGGLE_RAIL},
    {"switch", TOGGLE_SWITCH},
    {"invert", TOGGLE_INVERT},
};
static const int plcMsgTypeTableSize = 3;

static const KeywordToken resReqTypeTable[] = {{"lock", LOCK_RESOURCE},
                                               {"release", RELEASE_RESOURCE}};
static const int resReqTypeTableSize = 2;

int getTokenType(const char* word, const KeywordToken* keyword_table,
                 const int size) {
  for (int i = 0; i < size; i++) {
    if (strcmp(keyword_table[i].keyword, word) == 0) {
      return keyword_table[i].token;
    }
  }
  return -1;
}

int compare_ints(const void* a, const void* b) {
  int arg1 = *(const int*)a;
  int arg2 = *(const int*)b;

  if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}

char* readPathLine(Path_t path) {
  char* line = malloc(sizeof(char) * MAX_LINE_SIZE);
  return fgets(line, MAX_LINE_SIZE, path);
}

const char separator[] = " ";

const char* get_filename_ext(const char* filename) {
  const char* dot = strrchr(filename, '.');
  if (!dot || dot == filename) return "";
  return dot + 1;
}

Path_t initPath(const char* filePath) {
  const char* ext = get_filename_ext(filePath);
  if (strncmp(ext, "route", 5) != 0) {
    verbose("[Interpreter]: Parsing filename ... " VERBOSE_KRED
            "fail \n" VERBOSE_RESET);
  }

  verbose("[Interpreter]: Parsing filename ... " VERBOSE_KGRN
          "success \n" VERBOSE_RESET);
  FILE* fp = fopen(filePath, "r");
  return fp;
}

int destroyInterpreter(Path_t path) { return fclose(path); }

// Helper to handle the "set train id" command
int handleSetTrainId(char** nextToken, Train_t* state) {
  verbose("[Interpreter]: Setting Train Id ... \n");
  char* idStr = strtok_r(NULL, separator, nextToken);
  if (idStr == NULL) {
    verbose("[Interpreter]: Setting Train Id ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  int id = atoi(idStr);
  int res = setTrainId(state, id);
  if (res == -1) {
    verbose("[Interpreter]: Setting Train Id ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
  } else {
    verbose("[Interpreter]: Setting Train Id ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  }
  return res;
}

// Helper to handle the "PLC" command
int handlePlcCommand(char** nextToken, Train_t* state, PlcProxy_t* plc) {
  int res = 0;
  verbose("[Interpreter]: Contacting the PLC ... \n");
  char* plcMsgTypeStr = strtok_r(NULL, separator, nextToken);
  char* targetIdStr   = strtok_r(NULL, separator, nextToken);
  if (plcMsgTypeStr == NULL || targetIdStr == NULL) {
    verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  int plcMsgTypeInt = getTokenType(plcMsgTypeStr, plcMsgTypeTable, plcMsgTypeTableSize);
  if (plcMsgTypeInt == -1) {
    verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  PlcMessageType_e plcMsgType = (PlcMessageType_e)plcMsgTypeInt;

  uint16_t targetId;
  if (str_to_uint16(targetIdStr, &targetId) == -1) {
    verbose(VERBOSE_KRED "[Interpreter]: Error: invalid message type, not a valid integer\n" VERBOSE_RESET);
    return -1;
  }

  PlcMessage_t* msg = createPlcMessage();
  res = configWritePlcMessage(msg, plcMsgType, state->xwayStation, getTrainId(state), targetId);
  if (res == -1) {
    verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    free(msg);
    return -1;
  }

  ssize_t sentMsgSize = sendMessagePlcProxy(plc, msg, state->xwayStation);
  if (sentMsgSize == -1) {
    verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    free(msg);
    return -1;
  }

  PlcMessage_t* plcResp = readMessagePlcProxy(plc, getTrainId(state));
  if (plcResp == NULL) {
    verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    free(msg);
    return -1;
  }

  free(msg);
  free(plcResp);
  verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return res;
}

// Helper to handle the "resource" command
int handleResourceCommand(char** nextToken, Train_t* state, ResourceManagerProxy_t* resManager) {
  int res = 0;
  verbose("[Interpreter]: Contacting the Resource Manager ... \n");
  char* reqTypeStr = strtok_r(NULL, separator, nextToken);
  if (reqTypeStr == NULL) {
    verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  int reqTypeInt = getTokenType(reqTypeStr, resReqTypeTable, resReqTypeTableSize);
  if (reqTypeInt == -1) {
    verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  ResourceRequestType_e reqType = (ResourceRequestType_e)reqTypeInt;

  uint8_t resourceList[MAX_RESOURCE_REQUEST_AMM];
  memset(resourceList, 0, MAX_RESOURCE_REQUEST_AMM * sizeof(uint8_t));
  size_t len = 0;
  char* resIdStr = strtok_r(NULL, separator, nextToken);
  // At least one resource must be requested
  if (resIdStr == NULL) {
    verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  while (resIdStr) {
    int tmp = atoi(resIdStr);
    if (tmp < 0 || tmp > (int)MAX_RESOURCE) {
      res = -1;
      break;
    }

    resourceList[len] = (uint8_t)tmp;
    len += 1;

    if (len >= MAX_RESOURCE_REQUEST_AMM) {
      break;
    }

    resIdStr = strtok_r(NULL, separator, nextToken);
  }

  if (res == -1) {
    verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  qsort(resourceList, len, sizeof(int), compare_ints);
  for (size_t i = 0; i < len; i++) {
    res = requestResource(resManager, reqType, resourceList[i], getTrainId(state));
    if (res == -1) {
      verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
      return -1;
    }
  }
  verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return res;
}

// The refactored executeCommand function now dispatches to helper functions
int executeCommand(char* cmdLine, Train_t* state, PlcProxy_t* plc, ResourceManagerProxy_t* resManager) {
  verbose("[Interpreter]: Executing Command ... \n");
  if (cmdLine == NULL || state == NULL || plc == NULL || resManager == NULL) {
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  verbose("[Interpreter]: Parsing Line Command ... %s\n", cmdLine);
  char* nextToken = NULL;
  char* cmdStr = strtok_r(cmdLine, " ", &nextToken);
  if (cmdStr == NULL) {
    verbose("[Interpreter]: Parsing Line Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  int cmdInt = getTokenType(cmdStr, interpreterCommandTable, interpreterCommandTableSize);
  if (cmdInt == -1) {
    verbose("[Interpreter]: Parsing Line Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  InterpreterCommandType_e cmd = (InterpreterCommandType_e)cmdInt;
  verbose("[Interpreter]: Parsing Line Command ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  int res = -1;
  switch (cmd) {
    case CMD_SET_TRAIN_ID:
      res = handleSetTrainId(&nextToken, state);
      break;
    case CMD_SET:
      res = handlePlcCommand(&nextToken, state, plc);
      break;
    case CMD_RESOURCE:
      res = handleResourceCommand(&nextToken, state, resManager);
      break;
    default:
      verbose("[Interpreter]: Invalid command \n");
      break;
  }

  if (res == -1) {
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
  } else {
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  }
  return res;
}

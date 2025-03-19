#include "train_manager/interpreter.h"
#include "train_manager/train.h"
#include "plc/plc_message.h"
#include "plc/plc_facade.h"
#include "common/verbose.h"
#include <stdio.h>
#include <string.h>

#define MAX_LINE_SIZE 50
#define MAX_RESOURCE_REQUEST_AMM 10

static const KeywordToken interpreterCommandTable[] = {
    {"trainId", CMD_SET_TRAIN_ID},
    {"plc", CMD_SET},
    {"resource", CMD_RESOURCE}
};
static const int interpreterCommandTableSize = 3;

static const KeywordToken plcMsgTypeTable[] = {
    {"rail", TOGGLE_RAIL},
    {"switch", TOGGLE_SWITCH},
    {"invert", TOGGLE_TRAIN_DIR}
};
static const int plcMsgTypeTableSize = 3;

static const KeywordToken resReqTable[] = {
    {"lock", LOCK_RESOURCE},
    {"release", RELEASE_RESOURCE}
};
static const int resReqTableSize = 2;

int getTokenType(const char *word, const KeyWordToken* keyword_table, const int size) {
    for (int i = 0; i<size; i++) {
        if (strcmp(keyword_table[i].keyword, word) == 0) {
            return keyword_table[i].token;
        }
    }
    return -1;
}

int compare_ints(const void* a, const void* b){
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

char* readPathLine(Path_t path){
  char* line = malloc(sizeof(char)*MAX_LINE_SIZE);
  return fgets(line, MAX_LINE_SIZE, path);
}

int executeCommand(char* cmdLine, Train_t* state, PlcProxy_t* plc,
                   ResourceManagerProxy_t* resManager){
  verbose("[Interpreter]: Executing Command ... \n");
  if(cmdLine == NULL || state == NULL || plc == NULL || resManager == NULL){
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }

  verbose("[Interpreter]: Parsing Line Command ... \n");
  // split string in a thread safe way
  rsize_t strMax = sizeof(cmdLine);
  char* nextToken;
  char* cmdStr = strtok_r(cmdLine, &strMax, " ", &nextToken);
  if(cmdStr == NULL){
    verbose("[Interpreter]: Parsing Line Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;
  }
  int cmdInt = getTokenType(cmdStr,interpreterCommandTable, interpreterCommandTableSize);
  if(cmdInt == -1){
    verbose("[Interpreter]: Parsing Line Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
    return -1;

  }
  InterpreterCommandType_e cmd = (InterpreterCommandType_e)cmdInt;

  verbose("[Interpreter]: Parsing Line Command ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);

  int res = -1;
  switch(cmd){
    case(CMD_SET_TRAIN_ID):
      res = 0;
      verbose("[Interpreter]: Setting Train Id ... \n");

      char* idStr = strtok_r(NULL, &strMax, " ", &nextToken);
      if(idStr == NULL){
        verbose("[Interpreter]: Setting Train Id  ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }
      int id = atoi(idStr):
      res = setTrainId(state,id);

      if(res == -1){
        verbose("[Interpreter]: Setting Train Id  ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }

      verbose("[Interpreter]: Setting Train Id ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
      break;

    case(CMD_SET):
      res = 0;
      verbose("[Interpreter]: Contacting the PLC ... \n");
      char* plcMsgTypeStr = strtok_r(NULL,&strMax, " ", &nextToken);
      char* targetIdStr = strtok_r(NULL,&strMax, " ", &nextToken);

      if(plcMsgTypeStr == NULL || targetIdStr == NULL){
        verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }

      int plcMsgTypeInt = getTokenType(plcMsgTypeStr,plcMsgTypeTable, plcMsgTypeTableSize);

      if(plcMsgTypeInt == -1){
        verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }

      PlcMessageType_e plcMsgType = (PlcMessageType_e)plcMsgTypeInt;

      int targetId = atoi(targetIdStr);

      PlcMessage_t* msg;
      switch(plcMsgType){
        case TOGGLE_RAIL:
          res = createToggleRailStateMessage(msg,getTrainId(state),targetId);
          break;
        case TOGGLE_SWITCH:
          res = createToggleSwitchStateMessage(msg,getTrainId(state),targetId);
          break;
        case TOGGLE_TRAIN_DIR:
          res = createInvertDirectionMessage(msg,getTrainId(state),targetId);
          break;
      }

      if(res == -1){
        verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        free(msg);
        break;
      }

      res = sendMessagePlcProxy(plc, msg);

      if(res == -1){
        verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        free(msg);
        break;
      }

      PlcMessage_t* plcResp = sendMessagePlcProxy(plc, msg);

      if(plcResp == NULL){
        verbose("[Interpreter]: Contacting the PLC ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        free(msg);
        break;
      }

      // TODO: update Train state

      free(msg);
      free(plcResp);

      if(res == 0){
        verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
      }
      break;

    case(CMD_RESOURCE):
      res = 0;
      verbose("[Interpreter]: Contacting the Resource Manager ... \n");
      char* reqTypeStr = strtok_r(NULL,&strMax, " ", &nextToken);
      if(reqTypeStr == NULL){
        verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }

      int reqTypeInt = getTokenType(reqTypeStr,resReqTypeTable, resReqTypeTableSize);

      if(reqTypeInt == -1){
        verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }

      ResourceRequestType_e reqType = (ResourceRequestType_e)reqTypeInt;

      int resourceList[MAX_RESOURCE_REQUEST_AMM];
      int len = 0;
      char* resIdStr = strtok_r(NULL,&strMax, " ", &nextToken);

      // enforces that atleast one resource must be requested
      if(resIdStr == NULL){
        verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
        break;
      }

      while(resIdStr){
        resourceList[len] = atoi(resIdStr);
        len+=1;
        if(len >= MAX_RESOURCE_REQUEST_AMM){
          break;
        }
        resIdStr = strtok_r(NULL,&strMax, " ", &nextToken);
      }

      qsort(resourceList,len,sizeof(int),compare_ints);

      int reqRes = 0;
      for(int i=0; i<len; i++){
         reqRes = requestResource(resManager,reqType, resourceList[i], getTrainId(state));

        if(reqRes == -1){
          verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
          break;
        }

      }

      res = reqRes;
      if(res == 0){
        verbose("[Interpreter]: Contacting the Resource Manager ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
      }
      break;
  }

  if(res == -1){
    verbose("[Interpreter]: Executing Command ... " VERBOSE_KRED "fail \n" VERBOSE_RESET);
  }

  verbose("[Interpreter]: Executing Command ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return res;
}

Path_t initPath(char* filePath){
  //TODO: check file extension
  FILE* fp = fopen(filePath);
  return fp;
}

int destroyInterpreter(Path_t path){
  return fclose(path);
}

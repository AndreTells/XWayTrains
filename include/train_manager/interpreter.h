#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "train_manager/train.h"

typedef FILE* Path_t;

typedef enum {
  CMD_SET_TRAIN_ID, // 1 in data: id
  CMD_SET, // 2 in data: PlcMessageType, id
  CMD_RESOURCE, // at least 2 in: data ResourceRequestType_e, resourceId1, resourceId2 ...
  UNKOWN
}InterpreterCommandType_e ;

typedef struct {
    const char *keyword;
    int token;
} KeywordToken;


char* readPathLine(Path_t path);

int executeCommand(char* cmd, Train_t* state, PlcProxy_t* plc,
                   ResourceManagerProxy_t* resManager);

Path_t initPath(char* filePath);

int destroyInterpreter(Path_t path);

int executePlcCommand(PlcMessageType_e plcCommandType, int targetId);

int executeResourceManagerCommand(ResourceManagerProxy_t* resManager, int* resList, int listLen);

#endif // INTERPRETER_H_

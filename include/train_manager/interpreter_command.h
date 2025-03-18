#ifndef INTERPRETER_COMMAND_H_
#define INTERPRETER_COMMAND_H_
#include "train_manager/train.h"

typedef enum {
  CMD_SET_TRAIN_ID,
  CMD_SET_RAIL,
  CMD_SET_SWTICH,
  CMD_SET_INVERTER
}InterpreterCommandType_e ;

typedef int CmdFunction_t (Train_t* state);
typedef int CmdException_t (Interpreter_t* interpreter,Train_t* state);

typedef struct InterpreterCommand_t{
  InterpreterCommandType_e cmdType;
  CmdFunction_t execFunc;
  CmdException_t exceptFunc;
} InterpreterCommand_t;

InterperterCommand_t* getStandardCommand(InterpreterCommandType_e cmdType);

#endif // INTERPRETER_COMMAND_H_

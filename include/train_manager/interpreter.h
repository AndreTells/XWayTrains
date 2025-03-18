#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "train_manager/train.h"

typedef struct Path_t Path_t;

typedef struct Interpreter_t Interpreter_t;

char* readPathLine(Path_t* path);

int parseCommand(char* pathLine);

int executeCommand(Interpreter_t* interpreter, InterpreterCommand_t* cmd, Train_t* state);

Interpreter_t* initInterpreter();

int destroyInterpreter(Interpreter_t* interpreter);

Path_t* initPath(char* filePath);

int destroyInterpreter(Path_t* path);

#endif // INTERPRETER_H_

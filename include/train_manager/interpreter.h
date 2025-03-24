/**
 * @file interpreter.h
 * @brief Interface for interpreting and executing commands related to train
 * management.
 * @details Provides functions for reading and executing commands from a file,
 *          as well as managing train states and PLC communication.
 */
#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include <stdio.h>

#include "train_manager/train.h"

/**
 * @typedef Path_t
 * @brief Represents a file handle for reading command paths.
 */
typedef FILE* Path_t;

/**
 * @enum InterpreterCommandType_e
 * @brief Enumeration of command types for the interpreter.
 */
typedef enum {
  CMD_SET_TRAIN_ID,  // 1 in data: id
  CMD_SET,           // 2 in data: PlcMessageType, id
  CMD_RESOURCE,      // at least 2 in: data ResourceRequestType_e, resourceId1,
                     // resourceId2 ...
  UNKNOWN
} InterpreterCommandType_e;

/**
 * @struct KeywordToken
 * @brief Structure mapping command keywords to token values.
 * @note TODO: maybe refactor this so it's not exposed to all
 */
typedef struct {
  const char* keyword;
  int token;
} KeywordToken;

/**
 * @brief Reads a single line from the given path.
 *
 * @param path File handle to read from.
 * @return char* Pointer to the read line (must be freed by the caller), or NULL
 * on failure.
 */
char* readPathLine(Path_t path);

/**
 * @brief Executes a given command related to train state or PLC communication.
 *
 * @param cmd The command string to execute.
 * @param state Pointer to the train state to be modified.
 * @param plc Pointer to the PLC proxy for sending messages.
 * @param resManager Pointer to the resource manager proxy.
 * @return int Status code (0 on success, non-zero error code on failure).
 */
int executeCommand(char* cmdLine, Train_t* state, PlcProxy_t* plc, uint8_t XwayStation,
                   ResourceManagerProxy_t* resManager);
/**
 * @brief Initializes a file path for reading commands.
 *
 * @param filePath Path to the command file.
 * @return Path_t File handle for the initialized path, or NULL on failure.
 */
Path_t initPath(const char* filePath);

/**
 * @brief Closes and destroys the interpreter path.
 *
 * @param path File handle to close.
 * @return int Status code (0 on success, non-zero error code on failure).
 */
int destroyInterpreter(Path_t path);

#endif  // INTERPRETER_H_

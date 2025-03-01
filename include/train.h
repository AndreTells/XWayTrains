/**
 * @file train.h
 * @brief Everything pertinent to actions of the train thread
 */
#ifndef TRAIN_H_
#define TRAIN_H_
#include <pthread.h>

#include "model_info.h"
#include "plc_proxy.h"
#include "resource_manager_proxy.h"

/**
 * @brief Opaque handle for the Train instance
 * @details Encapsulates all train-related state and functionality.
 *          Implementation details are hidden in the corresponding .c file.
 */
typedef struct Train_t Train_t;

/**
 * @brief Initialize a new Train instance
 * @param[in] trainId Identifier for the train (from the TrainId_e enum)
 * @param[in] plc Proxy instance for communication with the PLC
 * @param[in] resManager Proxy instance for communication with the Resource
 * Manager
 * @param[in] routeFilePath File path to the train's route file
 * @return Pointer to the newly created Train_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endTrain()`.
 */
Train_t* initTrain(enum TrainId_e trainId, PlcProxy_t* plc,
                   ResourceManagerProxy_t* resManager, char* routeFilePath);

/**
 * @brief Gracefully terminate a Train instance
 * @param[in] train Train instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endTrain(Train_t* train);

/**
 * @brief Entry point for the train thread
 * @param[in] train Pointer to the Train_t instance
 * @return Thread exit status (always NULL)
 */
void* trainThread(Train_t* train);

#endif  // TRAIN_H_

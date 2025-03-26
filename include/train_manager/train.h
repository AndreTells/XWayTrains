/**
 * @file train.h
 * @brief Everything pertinent to actions of the train thread
 */
#ifndef TRAIN_H_
#define TRAIN_H_
#include <pthread.h>

#include "plc/model_info.h"
#include "plc/plc_proxy.h"
#include "train_manager/resource_manager_proxy.h"

/**
 * @brief Opaque handle for the Train instance
 * @details Encapsulates all train-related state and functionality.
 *          Implementation details are hidden in the corresponding .c file.
 */
typedef struct Train_t Train_t;

/**
 * @brief Initialize a new Train instance
 * @param[in] plc Proxy instance for communication with the PLC
 * @param[in] resManager Proxy instance for communication with the Resource
 * Manager
 * @param[in] routeFilePath File path to the train's route file
 * @return Pointer to the newly created Train_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endTrain()`.
 */
Train_t* initTrain(PlcProxy_t* plc, ResourceManagerProxy_t* resManager,
                   char* routeFilePath);

/**
 * @brief Gracefully terminate a Train instance
 * @param[in] train Train instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endTrain(Train_t* train);

/**
 * @brief Sets the ID of a train.
 *
 * @param[in] train Pointer to the train instance.
 * @param[in] id Numeric identifier for the train.
 * @return int Status code (0 on success, non-zero error code on failure).
 * @todo Update the ID type to use TrainId_e instead of an integer.
 */
int setTrainId(Train_t* train, int id);

/**
 * @brief Retrieves the ID of a train.
 *
 * @param[in] train Pointer to the train instance.
 * @return enum TrainId_e The train's ID.
 */
enum TrainId_e getTrainId(Train_t* train);

int executeRoute(Train_t* train, uint8_t station);
#endif  // TRAIN_H_

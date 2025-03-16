/**
 * @file train_manager_proxy.h
 * @brief Everything pertinent to communicating with the train from the resource
 *        manager
 */
#ifndef TRAIN_MANAGER_PROXY_H_
#define TRAIN_MANAGER_PROXY_H_

#include "common/comm_resource.h"

/**
 * @brief Opaque handle for Train Manager Proxy
 * @details Encapsulates connection state and communication details.
 *          Implementation is hidden in the .c file
 */
typedef struct TrainManagerProxy_t TrainManagerProxy_t;

/**
 * @brief Initialize a new Train Manager Proxy instance
 * @param[in] sockFd the file descriptor
 * @return Pointer to the newly created TrainManagerProxy_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endResourceManagerProxy()`.
 */
TrainManagerProxy_t* initTrainManagerProxy(int sockFd);

int endTrainManagerProxy(int sockFd);

ResourceRequest_t* getNewRequest(void);

int answerRequest(ResourceRequestResponse_t* response);

#endif  // TRAIN_MANAGER_PROXY_H_

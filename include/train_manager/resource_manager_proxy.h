/**
 * @file resource_manager_proxy.h
 * @brief Proxy for communicating with remote resource management system
 * @details Handles client-side communication and routing logic for resource
 *          allocation/deallocation in the remote Resource Manager.
 */
#ifndef RESOURCE_MANAGER_PROXY_H_
#define RESOURCE_MANAGER_PROXY_H_
#include "common/resource_request.h"

/**
 * @brief Opaque handle for Resource Manager Proxy
 * @details Encapsulates connection state and communication details.
 *          Implementation is hidden in the .c file
 */
typedef struct ResourceManagerProxy_t ResourceManagerProxy_t;

/**
 * @brief Initializes the Resource Manager Proxy.
 *
 * Creates and configures a connection to the remote Resource Manager.
 *
 * @param resManagerIpAddr IP address of the Resource Manager.
 * @param port Port number for communication.
 * @return ResourceManagerProxy_t* Pointer to the initialized proxy instance, or NULL on failure.
 */
ResourceManagerProxy_t* initResourceManagerProxy(char* resManagerIpAddr, int port);

/**
 * @brief Gracefully terminate a Resource Manager Proxy instance
 * @param[in] resManager Proxy instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endResourceManagerProxy(ResourceManagerProxy_t* resManager);

/**
 * @brief Sends a resource request to the Resource Manager.
 *
 * Handles requesting or releasing a resource on behalf of a client.
 *
 * @param[in] resManager Pointer to the Resource Manager Proxy instance.
 * @param[in] reqType Type of resource request (e.g., lock or release).
 * @param[in] resourceId Identifier of the resource being requested.
 * @param[in] clientId Identifier of the client making the request.
 * @return int Status code (0 on success, non-zero error code on failure).
 */
int requestResource(ResourceManagerProxy_t* resManager,
                    ResourceRequestType_e reqType, int resourceId,
                    int clientId);

#endif  // RESOURCE_MANAGER_PROXY_H_

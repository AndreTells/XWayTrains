/**
 * @file resource_manager_proxy.h
 * @brief Proxy for communicating with remote resource management system
 * @details Handles client-side communication and routing logic for resource
 *          allocation/deallocation in the remote Resource Manager.
 */
#ifndef RESOURCE_MANAGER_PROXY_H_
#define RESOURCE_MANAGER_PROXY_H_

/**
 * @brief Opaque handle for Resource Manager Proxy
 * @details Encapsulates connection state and communication details.
 *          Implementation is hidden in the .c file
 */
typedef struct ResourceManagerProxy_t ResourceManagerProxy_t;

/**
 * @brief Initialize a new Resource Manager Proxy instance
 * @param[in] resManagerIpAddr IP address of the resource Manager
 * @return Pointer to the newly created ResourceManagerProxy_t instance
 * @note The caller is responsible for gracefully terminating the instance using
 *       `endResourceManagerProxy()`.
 */
ResourceManagerProxy_t* initResourceManagerProxy(char* resManagerIpAddr);

/**
 * @brief Gracefully terminate a Resource Manager Proxy instance
 * @param[in] resManager Proxy instance handle to terminate
 * @return 0 on success, non-zero error code on failure
 */
int endResourceManagerProxy(ResourceManagerProxy_t* resManager);

/**
 * @brief Request a resource from the remote manager
 * @param[in] resManager Proxy instance handle
 * @param[in] resourceId ID of the resource to request
 * @param[in] clientId ID of the client requesting the resource
 * @return 0 on success, non-zero error code on failure
 */
int requestResource(ResourceManagerProxy_t* resManager, int resourceId,
                    int clientId);

/**
 * @brief Release a previously allocated resource
 * @param[in] resManager Proxy instance handle
 * @param[in] resourceId ID of the resource to release
 * @param[in] clientId ID of the client releasing the resource
 * @return 0 on success, non-zero error code on failure
 */
int releaseResource(ResourceManagerProxy_t* resManager, int resourceId,
                    int clientId);

#endif  // RESOURCE_MANAGER_PROXY_H_

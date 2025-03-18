/**
 * @file resource_manager.h
 * @brief Interface for a resource manager using the ResourceDataBaseProxy as its database.
 *
 * This file defines the interface for a resource manager that interacts with a
 * `ResourceDataBaseProxy_t` instance to manage resources. The resource manager ensures
 * remote access to the database and provides functionality for initialization,
 * termination, and accepting train managers.
 */

#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_
#include "resource_manager/resource_database_proxy.h"

/**
 * @brief Opaque handle for the Resource Database Proxy instance
 * @details Encapsulates all Resource Database Proxy information to make sure
 *          at most one modification can be made to the database at a time
 *          Implementation details are hidden in the corresponding .c file.
 */
typedef struct ResourceManager_t ResourceManager_t;

/**
 * @brief Initializes a new Resource Manager instance.
 *
 * This function creates and initializes a new Resource Manager instance, associating
 * it with the provided `ResourceDataBaseProxy_t` instance. The manager is also
 * configured with the specified IP address and port for communication.
 *
 * @param safeDatabase A pointer to the `ResourceDataBaseProxy_t` instance to be managed.
 * @param ipAddress The IP address to be used for communication.
 * @param port The port to be used for communication.
 * @return A pointer to the newly created Resource Manager instance, or NULL if initialization fails.
 */
ResourceManager_t* initResourceManager(ResourceDataBaseProxy_t* safeDatabase,
                                       char* ipAddress, int port);

/**
 * @brief Terminates a Resource Manager instance.
 *
 * This function safely shuts down and frees all resources associated with the
 * Resource Manager instance. It ensures that no further modifications are made
 * to the database by this manager.
 *
 * @param manager A pointer to the Resource Manager instance to be terminated.
 * @return 0 on success, or a non-zero value if an error occurs.
 */
int endResourceManager(ResourceManager_t* manager);

/**
 * @brief Accepts a train manager connection.
 *
 * This function allows the Resource Manager to accept and handle a connection
 * from a train manager. It ensures thread-safe access to the database during
 * the operation.
 *
 * @param manager A pointer to the Resource Manager instance.
 * @return 0 on success, or a non-zero value if an error occurs.
 */
int acceptTrainManager(ResourceManager_t* manager);

#endif  // RESOURCE_MANAGER_H_

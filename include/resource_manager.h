/**
 * @file resource_manager.h
 * @brief interface for a resource manager using the ResourceDataBaseProxy as
 *        its database
 */
#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_
#include "resource_database_proxy"

/**
 * @brief Opaque handle for the Resource Database Proxy instance
 * @details Encapsulates all Resource Database Proxy information to make sure
 *          at most one modification can be made to the database at a time
 *          Implementation details are hidden in the corresponding .c file.
 */
typedef struct ResourceManager_t ResourceManager_t;

ResourceManager_t* initResourceManager(RessourceDataBaseProxy_t* safeDatabase);

int endResourceManager(ResourceManager_t* manager);

int acceptTrainManager(ResourceManager_t* manager);

#endif // RESOURCE_MANAGER_H_

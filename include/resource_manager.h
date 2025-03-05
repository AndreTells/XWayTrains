#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_
#include "resource_database_proxy"

typedef struct ResourceManager_t ResourceManager_t;

ResourceManager_t* initResourceManager(RessourceDataBaseProxy_t* safeDatabase);

int endResourceManager(ResourceManager_t* manager);

int acceptTrainManager(ResourceManager_t* manager);

#endif // RESOURCE_MANAGER_H_

#ifndef RESSOURCE_DATABASE_PROXY_H_
#define RESSOURCE_DATABASE_PROXY_H_
#include <semaphore.h>

#include "resource_database.h"

typedef struct ResourceDataBaseProxy_t ResourceDataBaseProxy_t;

ResourceDataBaseProxy_t* initResourceDatabaseProxy(void);

int endResourceDataBaseProxy(ResourceDataBaseProxy_t* dbProxy);

int attemptLockResourceProxy(ResourceDataBaseProxy_t* db_proxy,
                              int ressourceId);

int releaseResourceProxy(ResourceDataBaseProxy_t* db_proxy, int ressourceId);

#endif  // RESSOURCE_DATABASE_PROXY_H_

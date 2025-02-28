#ifndef RESSOURCE_DATABASE_PROXY_H_
#define RESSOURCE_DATABASE_PROXY_H_
#include <semaphore.h>

#include "ressource_database.h"

typedef struct {
  RessourceDataBase_t* database;
  sem_t lock;
} RessourceDataBaseProxy_t;

RessourceDataBaseProxy_t* initRessourceDatabaseProxy(void);

int attemptLockRessourceProxy(RessourceDataBaseProxy_t* db_proxy,
                              int ressourceId);

int releaseRessourceProxy(RessourceDataBaseProxy_t* db_proxy, int ressourceId);

#endif  // RESSOURCE_DATABASE_PROXY_H_

#ifndef RESSOURCE_DATABASE_H_
#define RESSOURCE_DATABASE_H_
#include <semaphore.h>

typedef struct ResourceDataBase_t ResourceDataBase_t;

ResourceDataBase_t* initResourceDataBase(void);

int endResourceDataBase(ResourceDataBase_t* database);

int attemptLockResource(ResourceDataBase_t* database, int ressourceId);

int releaseResource(ResourceDataBase_t* database, int ressourceId);

#endif  // RESSOURCE_DATABASE_H_


#include <err.h>
#include <semaphore.h>
#include <stdlib.h>

#include "resource_database.h"
#include "resource_database_proxy.h"

struct ResourceDataBaseProxy_t{
  ResourceDataBase_t* database;
  sem_t lock;
};

ResourceDataBaseProxy_t* initResourceDatabaseProxy() {
  ResourceDataBaseProxy_t* db_proxy =
      (ResourceDataBaseProxy_t*)malloc(sizeof(ResourceDataBaseProxy_t));

  if (db_proxy == NULL) {
    err(EXIT_FAILURE, "malloc");
  }

  db_proxy->database = initResourceDataBase();

  if (sem_init(&db_proxy->lock, 0, 1) != 0) {
    err(EXIT_FAILURE, "sem_init");
  }

  return db_proxy;
}

int endResourceDataBaseProxy(ResourceDataBaseProxy_t* dbProxy){
  int resDb = endResourceDataBase(dbProxy->database);
  if(resDb != 0){
    return resDb;
  }
  free(dbProxy);

  return 0;
}

int attemptLockResourceProxy(ResourceDataBaseProxy_t* db_proxy,
                              int ressourceId) {
  sem_wait(&db_proxy->lock);
  if (!attemptLockResource(db_proxy->database, ressourceId)) {
    return -1;
  }

  sem_post(&db_proxy->lock);
  return 0;
}

int releaseResourceProxy(ResourceDataBaseProxy_t* db_proxy, int ressourceId) {
  sem_wait(&db_proxy->lock);

  int res = releaseResource(db_proxy->database, ressourceId);

  sem_post(&db_proxy->lock);
  return res;
}

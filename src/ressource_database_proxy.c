#include "ressource_database_proxy.h"

#include <err.h>
#include <semaphore.h>
#include <stdlib.h>

#include "ressource_database.h"

RessourceDataBaseProxy_t* initRessourceDatabaseProxy() {
  RessourceDataBaseProxy_t* db_proxy =
      (RessourceDataBaseProxy_t*)malloc(sizeof(RessourceDataBaseProxy_t));

  if (db_proxy == NULL) {
    err(EXIT_FAILURE, "malloc");
  }

  db_proxy->database = initRessourceDataBase();

  if (sem_init(&db_proxy->lock, 0, 1) != 0) {
    err(EXIT_FAILURE, "sem_init");
  }

  return db_proxy;
}

int attemptLockRessourceProxy(RessourceDataBaseProxy_t* db_proxy,
                              int ressourceId) {
  sem_wait(&db_proxy->lock);
  if (!attemptLockRessource(db_proxy->database, ressourceId)) {
    return -1;
  }

  sem_post(&db_proxy->lock);
  return 0;
}

int releaseRessourceProxy(RessourceDataBaseProxy_t* db_proxy, int ressourceId) {
  sem_wait(&db_proxy->lock);

  if (!releaseRessource(db_proxy->database, ressourceId)) {
    return -1;
  }

  sem_post(&db_proxy->lock);
  return 0;
}

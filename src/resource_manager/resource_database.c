#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource_database.h"

#define MAX_RESSOURCE_ID 50

struct {
  int registered[MAX_RESSOURCE_ID];
  sem_t* availability[MAX_RESSOURCE_ID];
} ResourceDataBase_t;

ResourceDataBase_t *initResourceDataBase() {
  ResourceDataBase_t *database =
      (ResourceDataBase_t *)malloc(sizeof(RessourceDataBase_t));

  if (database == NULL) {
    err(EXIT_FAILURE, "malloc");
  }

  memset(database->registered, 0, MAX_RESSOURCE_ID);
  return database;
}

int endResourceDataBase(ResourceDataBase_t* database){
  res = free(database);
  return res;
}

int attemptLockResource(ResourceDataBase_t *database, int ressourceId) {
  // ressource Id out of bounds
  if (ressourceId < 0 || ressourceId >= MAX_RESSOURCE_ID) {
    return -1;
  }

  // if ressource is not registered, register it
  if (!database->registered[ressourceId]) {
    database->registered[ressourceId] = 1;
    fflush(stdout);
    database->availability[ressourceId] = (sem_t *)malloc(sizeof(sem_t));
    sem_init(database->availability[ressourceId], 0, 1);
  }

  // TODO: not necessarily a sem_wait()
  // if not possible return erro
  // check if ressource is available
  int res = sem_wait(database->availability[ressourceId]);
  return res;
}

int releaseResource(ResourceDataBase_t *database, int ressourceId) {
  // if ressource is not registered, return error
  if (!database->registered[ressourceId]) {
    return -1;
  }

  return sem_post(database->availability[ressourceId]);
}

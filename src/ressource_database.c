#include "resource_database.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RessourceDataBase_t *initRessourceDataBase() {
  RessourceDataBase_t *database =
      (RessourceDataBase_t *)malloc(sizeof(RessourceDataBase_t));

  if (database == NULL) {
    err(EXIT_FAILURE, "malloc");
  }

  memset(database->registered, 0, MAX_RESSOURCE_ID);
  return database;
}

int attemptLockRessource(RessourceDataBase_t *database, int ressourceId) {
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

int releaseRessource(RessourceDataBase_t *database, int ressourceId) {
  // if ressource is not registered, return error
  if (!database->registered[ressourceId]) {
    return -1;
  }

  return sem_post(database->availability[ressourceId]);
}

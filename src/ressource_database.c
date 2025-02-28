#include "ressource_database.h"

#include <stdlib.h>
#include <string.h>

RessourceDataBase_t *init_db() {
  RessourceDataBase_t *database =
      (RessourceDataBase_t *)malloc(sizeof(RessourceDataBase_t));
  memset(database->registered, 0, MAX_RESSOURCE_ID);
  memset(database->availability, 0, MAX_RESSOURCE_ID);
  return database;
}

int attemptLockRessource(RessourceDataBase_t *database, int ressourceId) {
  // if ressource is not registered, register it
  if (database.registered[ressourceId]) {
    database.registered[ressourceId] = 1;
    database.availability[ressourceId] = 1;
  }

  // check if ressource is available
  if (database.availability[ressourceId]) {
    // lock ressource
    database.availability[ressourceId] -= 1;
    return 1;
  }

  // ressource was not available
  return 0;
}

int releaseRessource(RessourceDataBase_t *database, int ressourceId) {
  // if ressource is not registered, return error
  if (!database.registered[ressourceId]) {
    return -1;
  }

  // check if ressource is available
  database.availability[ressourceId] = 1;

  // ressource was not available
  return 1;
}

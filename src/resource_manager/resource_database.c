#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource_database.h"

#define MAX_RESSOURCE_ID 50

struct ResourceDataBase_t{
  int registered[MAX_RESSOURCE_ID];
  int availability[MAX_RESSOURCE_ID];
};

ResourceDataBase_t *initResourceDataBase() {
  ResourceDataBase_t *database =
      (ResourceDataBase_t *)malloc(sizeof(ResourceDataBase_t));

  if (database == NULL) {
    err(EXIT_FAILURE, "malloc");
  }

  memset(database->registered, 0, MAX_RESSOURCE_ID);
  memset(database->availability, 0, MAX_RESSOURCE_ID);
  return database;
}

int endResourceDataBase(ResourceDataBase_t* database){
  free(database);
  return 0;
}

int attemptLockResource(ResourceDataBase_t *database, int ressourceId) {
  // ressource Id out of bounds
  if (ressourceId < 0 || ressourceId >= MAX_RESSOURCE_ID) {
    return -1;
  }

  // if ressource is not registered, register it
  if (!database->registered[ressourceId]) {
    database->registered[ressourceId] = 1;
    database->availability[ressourceId] = 1;
  }

  if(database->availability[ressourceId] < 1){
    return -1;
  }

  database->availability[ressourceId] -= 1;
  return 0;
}

int releaseResource(ResourceDataBase_t *database, int ressourceId) {
  // if ressource is not registered, return error
  if (!database->registered[ressourceId]) {
    return -1;
  }
  database->availability[ressourceId] +=1;
  return 0;
}

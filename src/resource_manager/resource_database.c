
#include "resource_manager/resource_database.h"

#include <err.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESOURCE_ID 50

struct ResourceDataBase_t {
  bool registered[MAX_RESOURCE_ID];
  int availability[MAX_RESOURCE_ID];
  int owner[MAX_RESOURCE_ID];
  sem_t interest[MAX_RESOURCE_ID];
};

ResourceDataBase_t *initResourceDataBase() {
  ResourceDataBase_t *database =
      (ResourceDataBase_t *)malloc(sizeof(ResourceDataBase_t));

  if (database == NULL) {
    err(EXIT_FAILURE, "malloc");
  }

  (void)memset(database->registered, 0, MAX_RESOURCE_ID * sizeof(bool));
  (void)memset(database->availability, 0, MAX_RESOURCE_ID * sizeof(int));

  for (int i = 0; i < MAX_RESOURCE_ID; i++) {
    database->owner[i] = -1;
  }

  return database;
}

int endResourceDataBase(ResourceDataBase_t *database) {
  // destroy semaphores
  for (int i = 0; i < MAX_RESOURCE_ID; i++) {
    sem_destroy(&(database->interest[i]));
  }

  free(database);
  return 0;
}

int attemptLockResource(ResourceDataBase_t *database, int ressourceId,
                        int requesterId) {
  // ressource Id out of bounds
  if (ressourceId < 0 || ressourceId >= MAX_RESOURCE_ID) {
    return -1;
  }

  // if ressource is not registered, register it
  if (!database->registered[ressourceId]) {
    (void)registerResource(database, ressourceId, 1);
  }

  if (database->availability[ressourceId] < 1) {
    return -1;
  }

  database->availability[ressourceId] -= 1;
  database->owner[ressourceId] = requesterId;
  return 0;
}

int releaseResource(ResourceDataBase_t *database, int ressourceId,
                    int requesterId) {
  // only the owner can unlock it
  if (requesterId != database->owner[ressourceId]) {
    return -1;
  }

  // if ressource is not registered, return error
  if (!database->registered[ressourceId]) {
    return -1;
  }

  database->availability[ressourceId] += 1;

  (void)sem_post(&(database->interest[ressourceId]));
  return 0;
}

int waitResource(ResourceDataBase_t *database, int ressourceId) {
  return sem_wait(&(database->interest[ressourceId]));
}

int registerResource(ResourceDataBase_t *database, int ressourceId,
                     int ammount) {
  database->registered[ressourceId] = true;
  database->availability[ressourceId] = ammount;
  sem_init(&(database->interest[ressourceId]), 0, ammount);

  return 0;
}

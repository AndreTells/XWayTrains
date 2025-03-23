#include "resource_manager/resource_database.h"

#include <err.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/time_out.h"

#define MAX_RESOURCE_ID 50

struct ResourceDataBase_t {
  bool registered[MAX_RESOURCE_ID];
  unsigned int availability[MAX_RESOURCE_ID];
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

int attemptLockResource(ResourceDataBase_t *database, uint8_t resourceId,
                        int requesterId) {
  // resource Id out of bounds
  if (resourceId >= MAX_RESOURCE_ID) {
    return -1;
  }

  // if resource is not registered, register it
  if (!database->registered[resourceId]) {
    (void)registerResource(database, resourceId, 1);
  }

  if (database->availability[resourceId] < 1) {
    return -1;
  }

  database->availability[resourceId] -= 1;
  database->owner[resourceId] = requesterId;
  return 0;
}

int releaseResource(ResourceDataBase_t *database, uint8_t resourceId,
                    int requesterId) {
  // only the owner can unlock it
  if (requesterId != database->owner[resourceId]) {
    return -1;
  }

  // if resource is not registered, return error
  if (!database->registered[resourceId]) {
    return -1;
  }

  database->availability[resourceId] += 1;

  (void)sem_post(&(database->interest[resourceId]));
  return 0;
}

int waitResource(ResourceDataBase_t *database, uint8_t resourceId) {
  // if resource is not registered, register it
  if (!database->registered[resourceId]) {
    (void)registerResource(database, resourceId, 1);
  }

  struct timespec ts;
  loadTimeSpec(&ts);
  return sem_timedwait(&(database->interest[resourceId]), &ts);
}

int registerResource(ResourceDataBase_t *database, uint8_t resourceId,
                     unsigned int ammount) {
  if (ammount > 1) {
    // TODO: implement
    return -1;
  }
  database->registered[resourceId] = true;
  database->availability[resourceId] = ammount;
  sem_init(&(database->interest[resourceId]), 0, ammount);

  return 0;
}

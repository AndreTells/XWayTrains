#ifndef RESSOURCE_DATABASE_H_
#define RESSOURCE_DATABASE_H_

#define MAX_RESSOURCE_ID 50

typedef struct {
  int registered[MAX_RESSOURCE_ID];
  int availability[MAX_RESSOURCE_ID];
} RessourceDataBase_t;

RessourceDataBase_t* initRessourceDataBase();

int attemptLockRessource(RessourceDataBase_t* database, int ressourceId);

int releaseRessource(RessourceDataBase_t* database, int ressourceId);

#endif  // RESSOURCE_DATABASE_H_

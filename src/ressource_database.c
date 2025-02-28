#include <string.h>
#include <stdlib.h>

#include "ressource_database.h"

RessourceDataBase_t * init_db(){
  RessourceDataBase_t * db = (RessourceDataBase_t *) malloc(sizeof(RessourceDataBase_t));
  memset(db->registered,0,MAX_RESSOURCE_ID);
  memset(db->availability,0,MAX_RESSOURCE_ID);
  return db;
}

int attemptLockRessource(RessourceDataBase_t * db, int ressourceId){
  // if ressource is not registered, register it
  if(db.registered[ressourceId]){
    db.registered[ressourceId] = 1;
    db.availability[ressourceId] = 1;
  }

  // check if ressource is available
  if(db.availability[ressourceId]){
    // lock ressource
    db.availability[ressourceId] -= 1;
    return 1;
  }

  // ressource was not available
  return 0;
}

int releaseRessource(RessourceDataBase_t * db, int ressourceId){
  // if ressource is not registered, return error
  if(!db.registered[ressourceId]){
    return -1;
  }

  // check if ressource is available
  db.availability[ressourceId] = 1;

  // ressource was not available
  return 1;
}

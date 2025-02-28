#ifndef RESSOURCE_DATABASE_H_
#define RESSOURCE_DATABASE_H_

typedef struct{

} RessourceDataBase_t;

char* getRessourceNames(RessourceDataBase_t db);

int* getRessourceAvailability(RessourceDataBase_t db);

int lockRessourceByName(RessourceDataBase_t db, char* ressourceName);

int lockRessourceById(RessourceDataBase_t db, int ressourceId);

int releaseRessourceByName(RessourceDataBase_t db, char* ressourceName);

int releaseRessourceById(RessourceDataBase_t db, int ressourceId);

#endif // RESSOURCE_DATABASE_H_

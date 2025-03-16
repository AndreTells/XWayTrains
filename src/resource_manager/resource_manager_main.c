
#include "resource_manager/resource_database_proxy"
#include "resource_manager/resource_manager.h"

int main() {
  int socketFd = connectSocket();

  RessourceDataBaseProxy_t* safeDatabase = initRessourceDatabaseProxy();

  ResourceManager_t* manager = initResourceManager(safeDatabase);

  // TODO: add a way so the program can actually terminate

  while (!manager->finished) {
    (void)acceptTrainManager(manager);
  }

  (void)endResourceManager(manager);
  (void)endResourceDataBaseProxy(safeDatabase);
  return 0;
}

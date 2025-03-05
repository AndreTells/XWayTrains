
#include "resource_database_proxy"
#include "resource_manager.h"

int connectSocket();

int main() {
  int socketFd = connectSocket();

  RessourceDataBaseProxy_t* safeDatabase = initRessourceDatabaseProxy();

  ResourceManager_t* manager = initResourceManager(safeDatabase);

  // wait on possible connection
  pause();

  (void)endResourceManager(manager);
  (void)endResourceDataBaseProxy(safeDatabase);
  return 0;
}


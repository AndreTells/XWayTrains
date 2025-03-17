#include "resource_manager/resource_database_proxy.h"
#include "resource_manager/resource_manager.h"
#include <unistd.h>

int main() {
  // TODO: get actual IP

  ResourceDataBaseProxy_t* safeDatabase = initResourceDatabaseProxy();

  ResourceManager_t* manager = initResourceManager(safeDatabase, "127.0.0.1", 8080);

  // TODO: add a way so the program can actually terminate
  while (true) {
//    (void)acceptTrainManager(manager);
  }

  (void)endResourceManager(manager);
  (void)endResourceDataBaseProxy(safeDatabase);
  return 0;
}

#include <stdio.h>
#include <assert.h>

#include "resource_manager/resource_database.h"
#include "resource_manager/resource_database_proxy.h"
#include "common/flags.h"
#include "common/verbose.h"

int resource_database_test(void);
int resource_database_proxy_test(void);

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);
  verbose("[Unit Testing] Resource Manager ... \n\n");

  resource_database_test();
  resource_database_proxy_test();

  verbose("\n[Unit Testing] Resource Manager ... Done \n");
  return 0;
}

int resource_database_test(void) {
  verbose("[Resource Database] Init ... \n");
  ResourceDataBase_t* database = initResourceDataBase();
  assert(database != NULL);

  // Test resource registration
  assert(registerResource(database, 5, 2) == -1);
  assert(registerResource(database, 5, 1) == 0);
  assert(attemptLockResource(database, 5, 1) == 0);
  assert(attemptLockResource(database, 5, 2) == -1);
  assert(attemptLockResource(database, 5, 3) == -1); // No availability left

  // Test unlock behavior
  assert(releaseResource(database, 5, 1) == 0);
  assert(attemptLockResource(database, 5, 3) == 0); // Now should succeed
  assert(releaseResource(database, 5, 3) == 0);

  // Test invalid locks/unlocks
  assert(attemptLockResource(database, -1, 0) == -1);
  assert(attemptLockResource(database, 51, 0) == -1);
  assert(releaseResource(database, 5, 99) == -1); // Invalid owner

  // Test waiting for a resource
  assert(registerResource(database, 6, 1) == 0);
  assert(waitResource(database, 6) == 0); // Should block until released

  endResourceDataBase(database);
  verbose("[Resource Database] Init ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return 0;
}

int resource_database_proxy_test(void) {
  verbose("[Resource Database Proxy] Init ... \n");
  ResourceDataBaseProxy_t* database_proxy = initResourceDatabaseProxy();
  assert(database_proxy != NULL);

  endResourceDataBaseProxy(database_proxy);
  verbose("[Resource Database Proxy] Init ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
  return 0;
}

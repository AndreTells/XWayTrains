#include <stdio.h>

#include "resource_manager/resource_database.h"
#include "resource_manager/resource_database_proxy.h"

#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define RESET "\x1B[0m"

#define ALIGNMENT 43

// TODO(andre): refactor to a separate file
#define CHECK_LOG(str, expr)       \
  if ((expr) != 0) {               \
    printf("%*s", ALIGNMENT, str); \
    printf(KRED "Failed" RESET);   \
    printf("\n");                  \
  } else {                         \
    printf("%*s", ALIGNMENT, str); \
    printf(KGRN "Sucess" RESET);   \
    printf("\n");                  \
  }

int resource_database_test(void);
int resource_database_proxy_test(void);
int train_manager_proxy_test(void);

int main(void) {
  CHECK_LOG("Resource Database Test ... ", resource_database_test());
  printf("\n");

  CHECK_LOG("Resource Database Proxy Test ... ",
            resource_database_proxy_test());

  CHECK_LOG("Train Manager Proxy Test ... ", train_manager_proxy_test());

  return 0;
}

int resource_database_test(void) {
  ResourceDataBase_t* database = initResourceDataBase();
  CHECK_LOG("resource Database init function ... ", !(database));
  CHECK_LOG("attempt to lock random resource ... ",
            !(attemptLockResource(database, 10, 0) == 0));
  CHECK_LOG("attempt to lock first resource ... ",
            !(attemptLockResource(database, 0, 0) == 0));
  CHECK_LOG("attempt to lock last resource ... ",
            !(attemptLockResource(database, 10 - 1, 0) == 0));
  CHECK_LOG("attempt to lock invalid resource ... ",
            !(attemptLockResource(database, -1, 0) == -1));
  CHECK_LOG("attempt to lock invalid resource ... ",
            !(attemptLockResource(database, 10, 0) == -1));

  CHECK_LOG("attempt to unlock locked resource ... ",
            !(releaseResource(database, 10, 0) == 0));
  CHECK_LOG("attempt to unlock unlocked resource ... ",
            !(releaseResource(database, 3, 0) == -1));
  return 0;
}

int resource_database_proxy_test(void) {
  ResourceDataBaseProxy_t* database_proxy = initResourceDatabaseProxy();
  CHECK_LOG("resource Database Proxy init function ... ", !(database_proxy));
  return 0;
}
int train_manager_proxy_test(void) { return 0; }

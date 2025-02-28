#include <stdio.h>

#include "ressource_database.h"
#include "ressource_database_proxy.h"

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

int ressource_database_test(void);
int ressource_database_proxy_test(void);
int train_manager_proxy_test(void);

int main(void) {
  CHECK_LOG("Ressource Database Test ... ", ressource_database_test());
  printf("\n");

  CHECK_LOG("Ressource Database Proxy Test ... ",
            ressource_database_proxy_test());

  CHECK_LOG("Train Manager Proxy Test ... ", train_manager_proxy_test());

  return 0;
}

int ressource_database_test(void) {
  RessourceDataBase_t* database = initRessourceDataBase();
  CHECK_LOG("ressource Database init function ... ",
            !(database));
  CHECK_LOG("attempt to lock random ressource ... ",
            !(attemptLockRessource(database, 10) == 0));
  CHECK_LOG("attempt to lock first ressource ... ",
            !(attemptLockRessource(database, 0) == 0));
  CHECK_LOG("attempt to lock last ressource ... ",
            !(attemptLockRessource(database, MAX_RESSOURCE_ID - 1) == 0));
  CHECK_LOG("attempt to lock invalid ressource ... ",
            !(attemptLockRessource(database, -1) == -1));
  CHECK_LOG("attempt to lock invalid ressource ... ",
            !(attemptLockRessource(database, MAX_RESSOURCE_ID) == -1));

  CHECK_LOG("attempt to unlock locked ressource ... ",
            !(releaseRessource(database, 10) == 0));
  CHECK_LOG("attempt to unlock unlocked ressource ... ",
            !(releaseRessource(database, 3) == -1));
  return 0;
}

int ressource_database_proxy_test(void) {
  RessourceDataBaseProxy_t* database_proxy = initRessourceDatabaseProxy();
  CHECK_LOG("ressource Database Proxy init function ... ",
            !(database_proxy));
  return 0;
}
int train_manager_proxy_test(void) { return 0; }

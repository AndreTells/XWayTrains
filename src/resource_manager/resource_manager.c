#include <stdbool.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/queue.h> // struct with queue, see man tailq

#include "resource_manager.h"
#include "comm_general.h"

struct ResourceRequestQueueEntry_t{
  RessourceRequest_t* req;
  TAILQ_ENTRY(ResourceRequestQueueEntry_t) entries;
}

struct ResourceManager_t {
  bool finished;
  int listenFd;
  RessourceDataBaseProxy_t* safeDatabase;
  sem_t sendAccess;
  sem_t queueAccess;
  sem_t queueAvailability;
  TAILQ_HEAD(, ResourceRequestEntry) requestQueue;
};

//TODO: define struct for producer thread  data
//TODO: define struct for consumer thread  data

void* producerThread(void* data);
void* consumerThread(void* data);

ResourceManager_t* initResourceManager(RessourceDataBaseProxy_t* safeDatabase, char* ipAddress, int port){

  ResourceManager_t* manager = (ResourceManager_t*) malloc(sizeof(ResourceManager_t));

  if(manager == NULL){
    return NULL;
  }

  manager->safeDatabase = safeDatabase;
  manager->finished = false;
  TAILQ_INIT(&(manager->requestQueue))

  // initializing semaphores
  int resSemInit = sem_init(&(manager->queueAvailability), 0, 0);
  // check if sem_init failed
  if (resSemInit != 0) {
    free(resManager);
    return NULL;
  }

  int resSemInit2 = sem_init(&(manager->queueAccess), 0, 1);
  // check if sem_init failed
  if (resSemInit2 != 0) {
    sem_destroy(&(manager->queueAvailability));
    free(resManager);
    return NULL;
  }

  int resSemInit3 = sem_init(&(manager->sendAccess), 0, 1);
  // check if sem_init failed
  if (resSemInit3 != 0) {
    sem_destroy(&(manager->queueAvailability));
    sem_destroy(&(manager->queueAccess));
    free(resManager);
    return NULL;
  }

  // opening socket
  int listenFd = tcpCreateSocketWrapper(true, ipAddress, port);

  if(listenFd < 0){
    sem_destroy(&(manager->queueAvailability));
    sem_destroy(&(manager->queueAccess));
    sem_destroy(&(manager->sendAccess));
    free(resManager);
    return NULL;
  }

  manager->listenFd = listenFd;

  // create consumer thread

  return manager;
}

int endResourceManager(ResourceManager_t* manager){

  manager->finished = true;
  // join all threads

  close(listenFd);
  sem_destroy(&(manager->queueAvailability));
  sem_destroy(&(manager->queueAccess));
  sem_destroy(&(manager->sendAccess));

  (void)free(manager);
  return 0;
}

int acceptTrainManager(ResourceManager_t* manager){
  int connectionFd = tcpAcceptWrapper(manager->listenFd);
  if(connectionFd < 0){
    return -1;
  }

  // accepts a tcp connection
  // create a thread with the function producerThread

  return 0;
}

void* producerThread(void* data){
  pthread_exit(NULL);
}

void* consumerThread(void* data){
  pthread_exit(NULL);
}

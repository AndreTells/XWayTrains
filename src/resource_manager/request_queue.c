#include "resource_manager/request_queue.h"
#include "common/resource_request.h"
#include "common/time_out.h"

#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

typedef struct ResourceRequestQueueEntry_t{
  ResourceRequest_t* req;
  struct ResourceRequestQueueEntry_t* next;
  struct ResourceRequestQueueEntry_t* prev;
}ResourceRequestQueueEntry_t;

struct ResourceRequestQueue_t{
  sem_t lock;
  sem_t availability;
  ResourceRequestQueueEntry_t* tail;
  ResourceRequestQueueEntry_t* head;
};

ResourceRequestQueue_t* initQueue(){
    ResourceRequestQueue_t* queue = malloc(sizeof(ResourceRequestQueue_t));

    (void)sem_init(&(queue->lock),0,1);
    (void)sem_init(&(queue->availability),0,0);
    queue->tail = NULL;
    queue->head = NULL;

    return queue;
}

int destroyQueue(ResourceRequestQueue_t* queue){

  if(queue == NULL){
    return -1;
  }

  // clear out all requests still in the queue
  while(true){
    ResourceRequest_t* req = popQueue(queue);
    if(req == NULL){
      break;
    }

    int fd = req->returnFd;
    ResourceRequestResponse_t* resp = createResourceRequestResponse(req, RESOURCE_REFUSED);
    answerResourceRequest(fd, resp);
    free(req);
    free(resp);
  }

  sem_destroy(&(queue->lock));
  sem_destroy(&(queue->availability));

  free(queue);
  return 0;
}

int pushQueue(ResourceRequestQueue_t* queue, ResourceRequest_t* req){
  struct timespec ts;
  loadTimeSpec(&ts);
  int res;

  res = sem_timedwait(&(queue->lock),&ts);
  if(res < 0){
    return res;
  }


  ResourceRequestQueueEntry_t* newEntry = (ResourceRequestQueueEntry_t*) malloc(sizeof(ResourceRequestQueueEntry_t));
  if(newEntry == NULL){
    return -1;
  }

  newEntry->req = req;
  // place element at the end of the queue
  newEntry->prev = NULL;
  newEntry->next = queue->tail;

  // checking the edge case where the queue is empty
  if(queue->head == NULL){
    queue->head = newEntry;
  }
  if(queue->tail != NULL){
    queue->tail->prev = newEntry;
  }

  queue->tail = newEntry; // updating the end of the queue marker

  sem_post(&(queue->lock));
  sem_post(&(queue->availability));
  return 0;
}

ResourceRequest_t* popQueue(ResourceRequestQueue_t* queue){
  struct timespec ts;
  loadTimeSpec(&ts);
  int res;

  res = sem_timedwait(&(queue->availability), &ts);
  if(res<0){
    return NULL;
  }

  res = sem_timedwait(&(queue->lock), &ts);
  if(res<0){
    return NULL;
  }

  // checking edge case where queue is already emtpy
  if(queue->head == NULL){
    return NULL;
  }

  ResourceRequestQueueEntry_t* head = queue->head;
  ResourceRequestQueueEntry_t* newHead = head->prev;

  // setting the newHead as the head of the queue
  queue->head = newHead;

  // guarding against the case where the last element is removed
  if(newHead == NULL){
    queue->tail = NULL;
  }

  else{
    newHead->next = NULL;
  }

  // gettin the request at the old head
  ResourceRequest_t* req = head->req;

  free(head);

  res = sem_post(&(queue->lock));
  if(res < 0){
    return NULL;
  }
  return req;
}

#include "resource_manager/request_queue.h"
#include "common/resource_request.h"

#include <stdlib.h>

typedef sturct ResourceRequestQueueEntry_t{
  ResourceRequest_t* req;
  ResourceRequestQueueEntry_t* next;
  ResourceRequestQueueEntry_t* prev;
};

struct ResourceRequestQueue_t{
  sem_t lock;
  ResourceRequestQueueEntry_t* tail;
  ResourceRequestQueueEntry_t* head;
}

ResourceRequestQueue_t* initQueue(){
    ResourceRequestQueue_t* queue = (ResourceRequestQueue_t*) malloc(sizeof(ResourceRequest_t));
    (void)sem_init(&(queue->lock),0,1);
    queue->tail = NULL;
    queue->head = NULL;

    return queue;
}

int destroyQueue(ResourceRequestQueue_t* queue){

  // clear out all requests still in the queue
  while(queue->head != NULL){
    ResourceRequest_t* req = popQueue(queue);
    answerResourceRequest(req, -1);
  }

  //TODO: destroy semaphore

  return 0;
}

int pushQueue(ResourceRequestQueue_t* queue, ResourceRequest_t* req){
  ResourceRequestQueueEntry_t* newEntry = (ResourceRequestQueueEntry_t*) malloc(sizeof(ResourceRequestQueueEntry_t));
  sem_wait(&(queue->lock));

  if(newEntry == NULL){
    return -1;
  }

  newEntry->req = req;
  // place element at the end of the queue
  newEntry->prev = NULL;
  newEntry->next = queue->tail;
  queue->tail->prev = newEntry;

  queue->tail = newEntry; // updating the end of the queue marker

  // checking the edge case where the queue is empty
  if(queue->head == NULL){
    queue->head = newEntry;
  }

  sem_post(&(queue->lock));
  return 0;
}

ResourceRequest_t* popQueue(ResourceRequestQueue_t* queue){
  sem_wait(&(queue->lock));
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

  sem_post(&(queue->lock));
  return req;
}

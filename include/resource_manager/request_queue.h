#ifndef REQUEST_QUEUE_H_
#define REQUEST_QUEUE_H_
#include "common/resource_request.h"

// thread safe implementation of a queue
// allows for a blocking read and
// automatically 'serializes' the pushes
typedef struct ResourceRequestQueue_t ResourceRequestQueue_t;

ResourceRequestQueue_t* initQueue();

int destroyQueue(ResourceRequestQueue_t* queue);

int pushQueue(ResourceRequestQueue_t* queue, ResourceRequest_t* req);

ResourceRequest_t* popQueue(ResourceRequestQueue_t* queue);

#endif // REQUEST_QUEUE_H_

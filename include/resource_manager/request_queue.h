/**
 * @file request_queue.h
 * @brief Header file for a thread-safe queue implementation.
 *
 * This file defines the interface for a thread-safe queue that allows for
 * blocking reads and automatically serializes the pushes. The queue is designed
 * to hold `ResourceRequest_t` elements.
 */
#ifndef REQUEST_QUEUE_H_
#define REQUEST_QUEUE_H_
#include "common/resource_request.h"

/**
 * @brief Opaque structure representing a thread-safe queue.
 *
 * This structure is used to represent a thread-safe queue that holds
 * `ResourceRequest_t` elements.
 */
typedef struct ResourceRequestQueue_t ResourceRequestQueue_t;

/**
 * @brief Initializes a new queue.
 *
 * This function creates and initializes a new thread-safe queue.
 *
 * @return A pointer to the newly created queue, or NULL if the allocation fails.
 */
ResourceRequestQueue_t* initQueue();

/**
 * @brief Destroys a queue and frees all associated resources.
 *
 * This function safely destroys a queue and frees all associated resources.
 * It is the responsibility of the caller to ensure that no other threads are
 * using the queue when this function is called.
 *
 * @param queue A pointer to the queue to be destroyed.
 * @return 0 on success, or a non-zero value if an error occurs.
 */
int destroyQueue(ResourceRequestQueue_t* queue);

/**
 * @brief Pushes a new request onto the queue.
 *
 * This function adds a new `ResourceRequest_t` element to the queue. The operation
 * is thread-safe and will block if necessary to ensure proper serialization.
 *
 * @param queue A pointer to the queue where the request will be added.
 * @param req A pointer to the `ResourceRequest_t` to be added to the queue.
 * @return 0 on success, or a non-zero value if an error occurs.
 * @note waits at most 5 seconds to do it, otherwise returs an error
 */
int pushQueue(ResourceRequestQueue_t* queue, ResourceRequest_t* req);

/**
 * @brief Pops a request from the queue.
 *
 * This function removes and returns the next `ResourceRequest_t` element from the queue.
 * The operation is thread-safe and will block if the queue is empty until an element
 * becomes available.
 *
 * @param queue A pointer to the queue from which the request will be removed.
 * @return A pointer to the `ResourceRequest_t` that was removed from the queue, or NULL if an error occurs.
 * @note waits at most 5 seconds to do it, otherwise returs an error
 */
ResourceRequest_t* popQueue(ResourceRequestQueue_t* queue);

#endif // REQUEST_QUEUE_H_

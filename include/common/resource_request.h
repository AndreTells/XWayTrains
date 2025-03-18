/**
 * @file resource_request.h
 * @brief Header file for resource request handling.
 *
 * This file defines data structures and functions for managing resource requests,
 * including locking and releasing resources.
 */
#ifndef RESOURCE_REQUEST_H_
#define RESOURCE_REQUEST_H_

/**
 * @brief Enumeration for resource request types.
 */
typedef enum {
  LOCK_RESOURCE,
  RELEASE_RESOURCE
}ResourceRequestType_e;

/**
 * @brief Structure representing a resource request.
 */
typedef struct {
  int requesterId;
  int resourceId;
  ResourceRequestType_e reqType;
  int returnFd;
}ResourceRequest_t;

/**
 * @brief Structure representing a resource request.
 */
int sendResourceRequest(int fd, ResourceRequest_t* req);

/**
 * @brief Receives a resource request from a given file descriptor.
 *
 * @param fd File descriptor to receive the request from.
 * @return Pointer to a newly allocated ResourceRequest_t structure, or NULL on failure.
 * @note is limited by a time_out
 */
ResourceRequest_t* recvResourceRequest(int fd);

/**
 * @brief Sends a response to a resource request.
 *
 * @param req Pointer to the resource request structure.
 * @param resp Response value to send.
 * @return 0 on success, or a negative value on failure.
 */
int answerResourceRequest(ResourceRequest_t* req, int resp);

/**
 * @brief Creates a new resource request structure.
 *
 * @param requesterId ID of the requester.
 * @param resourceId ID of the requested resource.
 * @param reqType Type of resource request (lock/release).
 * @param fd File descriptor for returning response.
 * @return Pointer to a newly allocated ResourceRequest_t structure, or NULL on failure.
 */
ResourceRequest_t* createResourceRequest( int requesterId,int resourceId,
                                    ResourceRequestType_e reqType, int fd);

/**
 * @brief Destroys a resource request structure and frees its memory.
 *
 * @param req Pointer to the resource request structure to destroy.
 * @return 0 on success, or a negative value on failure.
 */
int destroyResourceRequest(ResourceRequest_t* req);

#endif // RESOURCE_REQUEST_H_

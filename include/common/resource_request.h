#ifndef RESOURCE_REQUEST_H_
#define RESOURCE_REQUEST_H_

#include <stdint.h>

#include "plc/model_info.h"

/**
 * @enum Resource_e
 * @brief Enumerates available resource identifiers.
 */
typedef enum { RESOURCE0, RESOURCE1, RESOURCE2, RESOURCE4, RESOURCE5, RESOURCE6} Resource_e;

/**
 * @brief Maximum resource available.
 */
static const Resource_e MAX_RESOURCE = RESOURCE6;

/**
 * @enum ResourceRequestType_e
 * @brief Enumerates types of resource requests.
 */
typedef enum {
  UNKNOWN_RES_REQ_TYPE = 0,
  LOCK_RESOURCE,
  RELEASE_RESOURCE
} ResourceRequestType_e;

/**
 * @enum ResourceRequestResponseType_e
 * @brief Enumerates possible responses to a resource request.
 */
typedef enum {
  UNKNOWN_RESP_REQ_TYPE = 0,
  RESOURCE_GRANTED,
  RESOURCE_REFUSED
} ResourceRequestResponseType_e;

/**
 * @struct ResourceRequest_t
 * @brief Structure representing a resource request.
 */
typedef struct {
  uint32_t requesterId;
  uint8_t resourceId;
  ResourceRequestType_e reqType;
  int returnFd;
} ResourceRequest_t;

/**
 * @struct ResourceRequestResponse_t
 * @brief Structure representing a response to a resource request.
 */
typedef struct {
  uint32_t requesterId;
  uint8_t resourceId;
  ResourceRequestResponseType_e respType;
} ResourceRequestResponse_t;

/**
 * @brief Sends a resource request.
 *
 * @param fd The file descriptor to send the request through.
 * @param req Pointer to the ResourceRequest_t structure containing the request
 * details.
 * @return int Status code (0 for success, negative for error).
 */
int sendResourceRequest(int fd, ResourceRequest_t* req);

/**
 * @brief Sends an answer to a resource request.
 *
 * @param fd The file descriptor to send the response through.
 * @param resp Pointer to the ResourceRequestResponse_t structure containing the
 * response details.
 * @return int Status code (0 for success, negative for error).
 */
int answerResourceRequest(int fd, ResourceRequestResponse_t* resp);

/**
 * @brief Receives a resource request.
 *
 * @param fd The file descriptor from which to receive the request.
 * @return ResourceRequest_t* Pointer to the received ResourceRequest_t
 * structure, or NULL on error.
 */
ResourceRequest_t* recvResourceRequest(int fd);

/**
 * @brief Receives a resource request response.
 *
 * @param fd The file descriptor from which to receive the response.
 * @return ResourceRequestResponse_t* Pointer to the received
 * ResourceRequestResponse_t structure, or NULL on error.
 */
ResourceRequestResponse_t* recvResourceRequestResponse(int fd);

/**
 * @brief Creates a new resource request.
 *
 * @param requesterId Unique identifier of the requester.
 * @param resourceId Identifier of the requested resource.
 * @param reqType Type of request (LOCK_RESOURCE or RELEASE_RESOURCE).
 * @param fd File descriptor for returning the response.
 * @return ResourceRequest_t* Pointer to the newly created ResourceRequest_t
 * structure.
 */
ResourceRequest_t* createResourceRequest(const enum TrainId_e requesterId,
                                         const uint8_t resourceId,
                                         ResourceRequestType_e reqType, int fd);
/**
 * @brief Destroys a resource request and frees associated memory.
 *
 * @param req Pointer to the ResourceRequest_t structure to be destroyed.
 * @return int Status code (0 for success, negative for error).
 */
int destroyResourceRequest(ResourceRequest_t* req);

/**
 * @brief Creates a new resource request response.
 *
 * @param req Pointer to the original ResourceRequest_t that is being responded
 * to.
 * @param respType Response type (RESOURCE_GRANTED or RESOURCE_REFUSED).
 * @return ResourceRequestResponse_t* Pointer to the newly created
 * ResourceRequestResponse_t structure.
 */
ResourceRequestResponse_t* createResourceRequestResponse(
    ResourceRequest_t* req, ResourceRequestResponseType_e respType);

/**
 * @brief Destroys a resource request response and frees associated memory.
 *
 * @param req Pointer to the ResourceRequestResponse_t structure to be
 * destroyed.
 * @return int Status code (0 for success, negative for error).
 */
int destroyResourceRequestResponse(ResourceRequestResponse_t* req);

#endif  // RESOURCE_REQUEST_H_

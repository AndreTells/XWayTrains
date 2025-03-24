#include "common/resource_request.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/time_out.h"
#include "common/verbose.h"
#include "plc/model_info.h"

#define RESOURCE_REQUEST_SERIALIZED_SIZE (sizeof(uint32_t) * 4)
#define RESOURCE_REQUEST_RESPONSE_SERIALIZED_SIZE (sizeof(uint32_t) * 3)

/**
 * @brief Serializes a ResourceRequest_t structure into a buffer.
 *
 * @param req Pointer to the ResourceRequest_t structure.
 * @param buffer Pointer to the buffer to fill with serialized data.
 * @return int 0 on success, negative on error.
 */
int serializeResourceRequest(const ResourceRequest_t* req, uint8_t* buffer) {
  if (!req || !buffer) {
    return -1;
  }
  size_t offset = 0;
  uint32_t net_val;

  // Serialize requesterId
  net_val = htonl(req->requesterId);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  // Serialize resourceId
  net_val = htonl(req->resourceId);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  // Serialize request type (reqType)
  net_val = htonl(req->reqType);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  // Serialize returnFd
  net_val = htonl((uint32_t)req->returnFd);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  return 0;
}

/**
 * @brief Deserializes a buffer into a ResourceRequest_t structure.
 *
 * @param buffer Pointer to the buffer containing serialized data.
 * @param req Pointer to the ResourceRequest_t structure to fill.
 * @return int 0 on success, negative on error.
 */
int deserializeResourceRequest(const uint8_t* buffer, ResourceRequest_t* req) {
  if (!req || !buffer) {
    return -1;
  }
  size_t offset = 0;
  uint32_t net_val;

  // Deserialize requesterId
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  req->requesterId = ntohl(net_val);
  offset += sizeof(net_val);

  // Deserialize resourceId
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  const uint32_t resourceId = ntohl(net_val);
  req->resourceId = resourceId > UINT8_MAX ? UINT8_MAX : (uint8_t)resourceId;
  offset += sizeof(net_val);

  // Deserialize request type (reqType)
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  req->reqType = (ResourceRequestType_e)ntohl(net_val);
  offset += sizeof(net_val);

  // Deserialize returnFd
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  req->returnFd = (int)ntohl(net_val);
  offset += sizeof(net_val);

  return 0;
}

/**
 * @brief Serializes a ResourceRequestResponse_t structure into a buffer.
 *
 * @param resp Pointer to the ResourceRequestResponse_t structure.
 * @param buffer Pointer to the buffer to fill with serialized data.
 * @return int 0 on success, negative on error.
 */
int serializeResourceRequestResponse(const ResourceRequestResponse_t* resp,
                                     uint8_t* buffer) {
  if (!resp || !buffer) {
    return -1;
  }
  size_t offset = 0;
  uint32_t net_val;

  // Serialize requesterId
  net_val = htonl(resp->requesterId);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  // Serialize resourceId
  net_val = htonl(resp->resourceId);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  // Serialize response type (respType)
  net_val = htonl(resp->respType);
  memcpy(buffer + offset, &net_val, sizeof(net_val));
  offset += sizeof(net_val);

  return 0;
}

/**
 * @brief Deserializes a buffer into a ResourceRequestResponse_t structure.
 *
 * @param buffer Pointer to the buffer containing serialized data.
 * @param resp Pointer to the ResourceRequestResponse_t structure to fill.
 * @return int 0 on success, negative on error.
 */
int deserializeResourceRequestResponse(const uint8_t* buffer,
                                       ResourceRequestResponse_t* resp) {
  if (!resp || !buffer) {
    return -1;
  }
  size_t offset = 0;
  uint32_t net_val;

  // Deserialize requesterId
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  resp->requesterId = ntohl(net_val);
  offset += sizeof(net_val);

  // Deserialize resourceId
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  const uint32_t resourceId = ntohl(net_val);
  resp->resourceId = resourceId > UINT8_MAX ? UINT8_MAX : (uint8_t)resourceId;
  offset += sizeof(net_val);

  // Deserialize response type (respType)
  memcpy(&net_val, buffer + offset, sizeof(net_val));
  resp->respType = (ResourceRequestResponseType_e)ntohl(net_val);
  offset += sizeof(net_val);

  return 0;
}

int sendResourceRequest(int fd, ResourceRequest_t* req) {
  verbose(
      "[RESOURCE REQUEST]: sending request from %d request type %d for "
      "resource %d through %d\n",
      req->requesterId, req->reqType, req->resourceId, fd);
  uint8_t buf[RESOURCE_REQUEST_SERIALIZED_SIZE];
  serializeResourceRequest(req, buf);

  int ret = (int)write(fd, buf, RESOURCE_REQUEST_SERIALIZED_SIZE);
  return ret;
}

int answerResourceRequest(int fd, ResourceRequestResponse_t* resp) {
  verbose(
      "[RESOURCE REQUEST]: sending response to %d resp type %d for resource %d "
      "through %d\n",
      resp->requesterId, resp->respType, resp->resourceId, fd);
  uint8_t buf[RESOURCE_REQUEST_RESPONSE_SERIALIZED_SIZE];
  serializeResourceRequestResponse(resp, buf);

  int ret = (int)write(fd, buf, RESOURCE_REQUEST_RESPONSE_SERIALIZED_SIZE);
  return ret;
}

ResourceRequest_t* recvResourceRequest(int fd) {
  int res;
  res = fileDescriptorTimedWait(fd);
  if (res < 0) {
    return NULL;
  }

  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));

  uint8_t buf[RESOURCE_REQUEST_SERIALIZED_SIZE];
  memset(buf, 0, RESOURCE_REQUEST_SERIALIZED_SIZE);

  res = (int)read(fd, buf, RESOURCE_REQUEST_SERIALIZED_SIZE);

  deserializeResourceRequest(buf, req);

  if (res == -1) {
    free(req);
    return NULL;
  }

  req->returnFd = fd;

  return req;
}

ResourceRequestResponse_t* recvResourceRequestResponse(int fd) {
  int res;
  res = fileDescriptorTimedWait(fd);
  if (res < 0) {
    return NULL;
  }

  ResourceRequestResponse_t* req = malloc(sizeof(ResourceRequest_t));

  uint8_t buf[RESOURCE_REQUEST_RESPONSE_SERIALIZED_SIZE];
  memset(buf, 0, RESOURCE_REQUEST_RESPONSE_SERIALIZED_SIZE);

  res = (int)read(
      fd, buf,
      RESOURCE_REQUEST_RESPONSE_SERIALIZED_SIZE);  // MAY CAUSE AN ISSUE

  deserializeResourceRequestResponse(buf, req);

  if (res == -1) {
    free(req);
    return NULL;
  }

  return req;
}

ResourceRequest_t* createResourceRequest(const enum TrainId_e requesterId,
                                         const uint8_t resourceId,
                                         ResourceRequestType_e reqType,
                                         int fd) {
  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  req->requesterId = requesterId;
  req->resourceId = resourceId;
  req->reqType = reqType;
  req->returnFd = fd;

  return req;
}

int destroyResourceRequest(ResourceRequest_t* req) {
  free(req);
  return 0;
}

ResourceRequestResponse_t* createResourceRequestResponse(
    ResourceRequest_t* req, ResourceRequestResponseType_e respType) {
  ResourceRequestResponse_t* resp = malloc(sizeof(ResourceRequestResponse_t));
  resp->requesterId = req->requesterId;
  resp->resourceId = req->resourceId;
  resp->respType = respType;

  return resp;
}

int destroyResourceRequestResponse(ResourceRequestResponse_t* req) {
  free(req);
  return 0;
}

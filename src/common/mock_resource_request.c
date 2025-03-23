#include <stdlib.h>
#include <sys/socket.h>

#include "common/resource_request.h"

int sendResourceRequest([[maybe_unused]] int fd,
                        [[maybe_unused]] ResourceRequest_t* req) {
  return 0;
}

ResourceRequest_t* recvResourceRequest(int fd) {
  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  req->requesterId = 0;
  req->resourceId = 0;
  req->reqType = LOCK_RESOURCE;
  req->returnFd = fd;

  return req;
}

ResourceRequestResponse_t* recvResourceRequestResponse(
    [[maybe_unused]] int fd) {
  ResourceRequest_t* req = createResourceRequest(0, 0, LOCK_RESOURCE, -1);
  ResourceRequestResponse_t* resp =
      createResourceRequestResponse(req, RESOURCE_GRANTED);
  free(req);
  return resp;
}

int answerResourceRequest([[maybe_unused]] int fd,
                          [[maybe_unused]] ResourceRequestResponse_t* resp) {
  return 0;
}

ResourceRequest_t* createResourceRequest(int requesterId, int resourceId,
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

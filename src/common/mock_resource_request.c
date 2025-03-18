#include "common/resource_request.h"
#include <stdlib.h>
#include <sys/socket.h>


int sendResourceRequest(int fd,ResourceRequest_t* req){
  return 0;
}

ResourceRequest_t* recvResourceRequest(int fd){

  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  req->requesterId = 0;
  req->resourceId = 0;
  req->reqType = LOCK_RESOURCE;
  req->returnFd = fd;

  return req;
}

int answerResourceRequest(ResourceRequest_t* req, int resp){
  return 0;
}

ResourceRequest_t* createResourceRequest( int requesterId,int resourceId,
                                    ResourceRequestType_e reqType, int fd){

  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  req->requesterId = requesterId;
  req->resourceId = resourceId;
  req->reqType = reqType;
  req->returnFd = fd;

  return req;
}

int destroyResourceRequest(ResourceRequest_t* req){
  free(req);
  return 0;
}

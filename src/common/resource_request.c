#include "common/resource_request.h"
#include <stdlib.h>
#include <sys/socket.h>

ResourceRequest_t* createResourceRequest( int requesterId,int resourceId,
                                    ResourceRequestType_e reqType, int fd);

int destroyResourceRequest(ResourceRequest_t* req);

int sendResourceRequest(int fd,ResourceRequest_t* req){
  (void)send(fd,&req,sizeof(ResourceRequest_t),0);
  return 0;
}

ResourceRequest_t* recvResourceRequest(int fd){

  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  recv(fd, req, sizeof(ResourceRequest_t), 0);
  req->returnFd = fd;

  return req;
}

int answerResourceRequest(ResourceRequest_t* req, int resp){
  (void)send(req->returnFd,&resp,sizeof(int),0);
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

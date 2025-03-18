#include "common/resource_request.h"
#include "common/time_out.h"
#include <stdlib.h>
#include <sys/socket.h>

int sendResourceRequest(int fd,ResourceRequest_t* req){
  (void)send(fd,&req,sizeof(ResourceRequest_t),0);
  return 0;
}

ResourceRequest_t* recvResourceRequest(int fd){

  int res;
  res = fileDescriptorTimedWait(fd);
  if(res <=0 ){
    return NULL;
  }

  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  res = recv(fd, req, sizeof(ResourceRequest_t), 0);

  if(res == -1){
    free(req);
    return NULL;
  }

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

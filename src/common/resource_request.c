#include "common/resource_request.h"
#include "common/time_out.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int sendResourceRequest(int fd,ResourceRequest_t* req){
  (void)write(fd,&req,sizeof(ResourceRequest_t));
  return 0;
}

int answerResourceRequest(int fd, ResourceRequestResponse_t* resp){
  (void)write(fd,&resp,sizeof(ResourceRequestResponse_t));
  return 0;
}

ResourceRequest_t* recvResourceRequest(int fd){

  int res;
  res = fileDescriptorTimedWait(fd);
  if(res <= 0 ){
    return NULL;
  }

  ResourceRequest_t* req = malloc(sizeof(ResourceRequest_t));
  res = read(fd, req, sizeof(ResourceRequest_t)); // MAY CAUSE AN ISSUE

  if(res == -1){
    free(req);
    return NULL;
  }

  req->returnFd = fd;

  return req;
}

ResourceRequestResponse_t* recvResourceRequestResponse(int fd){

  int res;
  res = fileDescriptorTimedWait(fd);
  if(res <= 0 ){
    return NULL;
  }

  ResourceRequestResponse_t* req = malloc(sizeof(ResourceRequest_t));
  res = read(fd, req, sizeof(ResourceRequestResponse_t));// MAY CAUSE AN ISSUE

  if(res == -1){
    free(req);
    return NULL;
  }

  return req;
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

ResourceRequestResponse_t* createResourceRequestResponse( ResourceRequest_t*req,
                                             ResourceRequestResponseType_e respType){

  ResourceRequestResponse_t* resp = malloc(sizeof(ResourceRequestResponse_t));
  resp->requesterId = req->requesterId;
  resp->resourceId = req->resourceId;
  resp->respType = respType;

  return resp;
}

int destroyResourceRequestResponse(ResourceRequestResponse_t* req){
  free(req);
  return 0;
}

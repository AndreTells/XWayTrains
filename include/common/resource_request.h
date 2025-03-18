#ifndef RESOURCE_REQUEST_H_
#define RESOURCE_REQUEST_H_

typedef enum {
  LOCK_RESOURCE,
  RELEASE_RESOURCE
}ResourceRequestType_e;

typedef struct {
  int requesterId;
  int resourceId;
  ResourceRequestType_e reqType;
  int returnFd;
}ResourceRequest_t;

int sendResourceRequest(int fd, ResourceRequest_t* req);

ResourceRequest_t* recvResourceRequest(int fd);

int answerResourceRequest(ResourceRequest_t* req, int resp);

ResourceRequest_t* createResourceRequest( int requesterId,int resourceId,
                                    ResourceRequestType_e reqType, int fd);

int destroyResourceRequest(ResourceRequest_t* req);

#endif // RESOURCE_REQUEST_H_

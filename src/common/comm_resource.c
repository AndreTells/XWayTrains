#include "comm_resource.h"

typedef struct {
  int trainId;
  int ressourceId;
} RessourceRequest_t;

typedef struct {
  int trainId;
  enum RequestResponse_e resp;
} ResourceRequestResponse_t;

ResourceRequest_t* createResourceRequest(enum TrainId_e trainId,
                                         int ressourceId){
  return NULL;
}

int destroyResourceRequest(ResourceRequest_t* req){
  return -1;
}

ResourceRequestResponse_t* createResourceRequestResponse(enum TrainId_e trainId,
                                                         int ressourceId){

  return NULL;
}

int destroyResourceRequestResponse(ResourceRequestResponse_t* resp){
  return -1;
}

unsigned char* serializeResourceRequest(ResourceRequest_t* request){
  return NULL;
}

ResourceRequest_t* unserializeResourceRequest(unsigned char* request){
  return NULL;
}

unsigned char* serializeResourceRequestResponse(ResourceRequestResponse_t* request){
  return NULL;
}

ResourceRequestResponse_t* unserializeResourceRequestResponse(unsigned char* request){
  return NULL;
}

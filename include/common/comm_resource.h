#ifndef COMM_RESSOURCE_H_
#define COMM_RESSOURCE_H_
#include "model_info.h"

enum RequestResponse_e { GRANTED, REFUSED, UNDEFINED };

typedef struct ResourceRequest_t ResourceRequest_t;

typedef struct ResourceRequestResponse_t ResourceRequestResponse_t;

ResourceRequest_t* createResourceRequest(enum TrainId_e trainId,
                                         int ressourceId);

int destroyResourceRequest(ResourceRequest_t* req);

ResourceRequestResponse_t* createResourceRequestResponse(enum TrainId_e trainId,
                                                         int ressourceId);

int destroyResourceRequestResponse(ResourceRequestResponse_t* resp);

unsigned char* serializeResourceRequest(ResourceRequest_t* request);

ResourceRequest_t* unserializeResourceRequest(unsigned char* request);

unsigned char* serializeResourceRequestResponse(
    ResourceRequestResponse_t* request);

ResourceRequestResponse_t* unserializeResourceRequestResponse(
    unsigned char* request);

#endif  // COMM_RESSOURCE_H_

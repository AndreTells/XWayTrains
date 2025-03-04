#ifndef COMM_RESSOURCE_H_
#define COMM_RESSOURCE_H_
#include "model_info.h"

enum RequestResponse_e { GRANTED, REFUSED, UNDEFINED };

typedef struct ResourceRequest_t ResourceRequest_t;

typedef struct ResourceRequestResponse_t ResourceRequestResponse_t;

ResourceRequest_t* createResourceRequest(enum TrainId_e trainId, int ressourceId);

void destroyResourceRequest(ResourceRequest_t* req);

ResourceRequestResponse_t* createResourceRequestResponse(enum TrainId_e trainId, int ressourceId);

void destroyResourceRequestResponse(ResourceRequestResponse_t* resp);

/*

typedef struct {
  int trainId;
  int* ressourceId;
  struct sockaddr* trainAddress;
} RessourceRequest_t;

typedef struct {
  int trainId;
  enum RequestResponse_e resp;
} RessourceRequestAnswer_t;
*/

#endif // COMM_RESSOURCE_H_

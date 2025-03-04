#ifndef TRAIN_MANAGER_PROXY_H_
#define TRAIN_MANAGER_PROXY_H_

#include "comm_resource.h"

ResourceRequest_t* getNewRequest(void);

int answerRequest(ResourceRequestResponse_t* response);

#endif  // TRAIN_MANAGER_PROXY_H_

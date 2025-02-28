#ifndef TRAIN_MANAGER_PROXY_H_
#define TRAIN_MANAGER_PROXY_H_

enum RequestResponse_e {
  GRANTED,
  REFUSED,
  UNDEFINED
};

typedef struct {
  int trainId;
  int* ressourceId;
  struct sockaddr * trainAddress;
} RessourceRequest_t;

typedef struct {
  int trainId;
  enum RequestResponse_e resp;
} RessourceRequestAnswer_t;


RessourceRequest_t * getNewRequest(void);

int answerRequest(RessourceRequest_t * request, enum RequestResponse_e response);

#endif // TRAIN_MANAGER_PROXY_H_

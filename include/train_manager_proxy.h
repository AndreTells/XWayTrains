#ifndef TRAIN_MANAGER_PROXY_H_
#define TRAIN_MANAGER_PROXY_H_

typedef struct {

} RessourceRequest_t;

enum RequestResponse_e {
  GRANTED,
  REFUSED
};

RessourceRequest_t* GetNewRequest();

int AnswerRequest(enum RequestResponse_e response);

#endif // TRAIN_MANAGER_PROXY_H_

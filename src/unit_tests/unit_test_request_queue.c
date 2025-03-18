#include <stdio.h>
#include <assert.h>
#include "resource_manager/request_queue.h"
#include "common/resource_request.h"
#include "common/verbose.h"
#include "common/flags.h"

void test_init_destroy_queue() {
  verbose("[Request Queue] Init Destroy ... \n");
  ResourceRequestQueue_t* queue = initQueue();

  int destroyRes = destroyQueue(queue);
  assert(destroyRes == 0);

  verbose("[Request Queue] Init Destroy ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

void test_push_pop_queue() {
  verbose("[Request Queue] Push Pop ... \n");
  ResourceRequestQueue_t* queue = initQueue();

  // Create a dummy request
  ResourceRequest_t req1, req2;

  assert(pushQueue(queue, &req1) == 0);
  assert(pushQueue(queue, &req2) == 0);

  // Pop first request
  ResourceRequest_t* poppedReq = popQueue(queue);
  assert(poppedReq == &req1);

  // Pop second request
  poppedReq = popQueue(queue);
  assert(poppedReq == &req2);

  int destroyRes = destroyQueue(queue);
  assert(destroyRes == 0);

  verbose("[Request Queue] Push Pop ... " VERBOSE_KGRN "success \n" VERBOSE_RESET);
}

int main(int argc, char* argv[]) {
  bool verbose_mode = get_flag_value(argc,argv, VERBOSE_FLAG, NULL);
  setVerbose(verbose_mode);

  verbose("[Unit Testing] Request Queue ... \n\n");
  test_init_destroy_queue();
  test_push_pop_queue();

  verbose("\n[Unit Testing] Request Queue ... Done \n");
  return 0;
}

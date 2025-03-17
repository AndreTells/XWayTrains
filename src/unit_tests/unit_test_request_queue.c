#include <stdio.h>
#include <assert.h>
#include "resource_manager/request_queue.h"
#include "common/resource_request.h"

void test_init_destroy_queue() {
    ResourceRequestQueue_t* queue = initQueue();

    int res = destroyQueue(queue);
    assert(res == 0);

    int destroyRes = destroyQueue(queue);
    assert(destroyRes == 0);

    printf("test_init_destroy_queue passed!\n");
}

void test_push_pop_queue() {
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

    destroyQueue(queue);

    int destroyRes = destroyQueue(queue);
    assert(destroyRes == 0);

    printf("test_push_pop_queue passed!\n");
}

int main() {
    test_init_destroy_queue();
    test_push_pop_queue();
    printf("All tests passed!\n");
    return 0;
}

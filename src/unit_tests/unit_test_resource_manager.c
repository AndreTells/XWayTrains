#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "resource_manager/resource_manager.h"
#include "resource_manager/request_queue.h"
#include "common/resource_request.h"

// Test initialization
void test_initResourceManager() {
    ResourceManager_t* manager = initResourceManager(NULL, "127.0.0.1", 8080);
    assert(manager != NULL);
    printf("[PASS] initResourceManager\n");
    endResourceManager(manager);
}

// Test accepting a client
void test_acceptTrainManager() {
    ResourceManager_t* manager = initResourceManager(NULL, "127.0.0.1", 8080);
    assert(manager != NULL);

    int result = acceptTrainManager(manager);
    assert(result == 0);

    printf("[PASS] acceptTrainManager\n");
    endResourceManager(manager);
}

// Test resource manager cleanup
void test_endResourceManager() {
    ResourceManager_t* manager = initResourceManager(NULL, "127.0.0.1", 8080);
    assert(manager != NULL);

    endResourceManager(manager);

    printf("[PASS] endResourceManager\n");
}

int main() {
    test_initResourceManager();
    test_acceptTrainManager();
//    TODO: find out how to test these
//    test_producerThread();
//    test_consumerThread();
    test_endResourceManager();

    printf("All tests passed!\n");
    return 0;
}

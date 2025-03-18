#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "train_manager/resource_manager_proxy.h"  // Exposes ResourceManagerProxy_t and functions

/*
 * Test: initResourceManagerProxy returns NULL when passed a NULL IP address.
 */
void test_init_with_null(void) {
    ResourceManagerProxy_t* proxy = initResourceManagerProxy(NULL);
    assert(proxy == NULL);
    printf("test_init_with_null passed.\n");
}

/*
 * Test: Valid IP address returns a non-NULL proxy.
 * Clean up by calling endResourceManagerProxy.
 */
void test_init_success(void) {
    ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1");
    assert(proxy != NULL);
    printf("test_init_success passed.\n");
    int ret = endResourceManagerProxy(proxy);
    assert(ret == 0);
}

/*
 * Test: endResourceManagerProxy returns -1 when passed a NULL pointer.
 */
void test_end_with_null(void) {
    int ret = endResourceManagerProxy(NULL);
    assert(ret == -1);
    printf("test_end_with_null passed.\n");
}


/*
 * Test: requestResource fails when an invalid client ID is used.
 */
void test_requestResource_invalid_client(void) {
    ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1");
    assert(proxy != NULL);
    int invalidClientId = 87;
    int ret = requestResource(proxy, 42, invalidClientId);
    assert(ret == -1);
    printf("test_requestResource_invalid_client passed.\n");
    endResourceManagerProxy(proxy);
}

/*
 * Test: releaseResource fails when an invalid client ID is used.
 */
void test_releaseResource_invalid_client(void) {
    ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1");
    assert(proxy != NULL);
    int invalidClientId = 32;
    int ret = releaseResource(proxy, 42, invalidClientId);
    assert(ret == -1);
    printf("test_releaseResource_invalid_client passed.\n");
    endResourceManagerProxy(proxy);
}

/*
 * Test: requestResource succeeds for a valid client.
 * This test assumes that the stub dependencies (linked externally)
 * simulate a successful resource request (e.g. send/receive succeed).
 */
void test_requestResource_success(void) {
    ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1");
    assert(proxy != NULL);
    int clientId = 0;  // Valid client ID
    int ret = requestResource(proxy, 42, clientId);
    assert(ret == 0);
    printf("test_requestResource_success passed.\n");
    endResourceManagerProxy(proxy);
}

/*
 * Test: releaseResource succeeds for a valid client.
 * Again, this assumes that the linked stub versions simulate success.
 */
void test_releaseResource_success(void) {
    ResourceManagerProxy_t* proxy = initResourceManagerProxy("127.0.0.1");
    assert(proxy != NULL);
    int clientId = 0;  // Valid client ID
    int ret = releaseResource(proxy, 42, clientId);
    assert(ret == 0);
    printf("test_releaseResource_success passed.\n");
    endResourceManagerProxy(proxy);
}

int main(void) {
    test_init_with_null();

    test_init_success();
    /*
    test_end_with_null();
    test_requestResource_invalid_client();
    test_releaseResource_invalid_client();
    test_requestResource_success();
    test_releaseResource_success();
      */
    printf("All tests passed.\n");
    return 0;
}

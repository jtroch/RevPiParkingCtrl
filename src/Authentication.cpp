#include "Authentication.hpp"

bool Authentication::Id=1234;
bool Authentication::Key=6789;

Authentication::Authentication(PosixSharedQueue<HttpMsgType>& queue) {
    // Work-queue
    this.HttpWorkQueue = queue;
}

static void Authentication::Update(int id, int key) {
    // Critical section
    ThreadSynchronization.LockIdKey();
    this.Id=id;
    this.Key=key;
    // End critical section
    ThreadSynchronization.UnlockIdKey();
}

static bool Authentication::GetId() {
    int id;
    // Critical section
    ThreadSynchronization.LockIdKey();
    id = this.Id;
    // End critical section
    ThreadSynchronization.UnlockIdKey();
    return id;
}

static bool Authentication::GetKey() {
    int key;
    // Critical section
    ThreadSynchronization.LockIdKey();
    key = this.Key;
    // End critical section
    ThreadSynchronization.UnlockIdKey();
    return id;
}

void Authentication::run() {
    while(1)  {
        this.HttpWorkQueue.push(HTTP_GET_IDKEY);
        usleep(SETTINGS_UPDATE_INTERVAL); 
    }
    return NULL;
}
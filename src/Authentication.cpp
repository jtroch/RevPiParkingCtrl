#include "RevPiParkingCtrl.hpp"
#include "ThreadSynchronization.hpp"
#include "Authentication.hpp"
#include "Http.hpp"
#include <syslog.h>

int Authentication::Id=1234;
int Authentication::Key=6789;

Authentication::Authentication() {
    // get a connection object
    HttpConnection = new RestClient::Connection(SERVER_ADDRESS);
}

void Authentication::Update(int id, int key) {
    // Critical section
    ThreadSynchronization::getInstance()->LockIdKey();
    Id=id;
    Key=key;
    // End critical section
    ThreadSynchronization::getInstance()->UnlockIdKey();
}

int Authentication::GetId() {
    int id;
    // Critical section
   ThreadSynchronization::getInstance()->LockIdKey();
    id = Id;
    // End critical section
    ThreadSynchronization::getInstance()->UnlockIdKey();
    return id;
}

 int Authentication::GetKey() {
    int key;
    // Critical section
    ThreadSynchronization::getInstance()->LockIdKey();
    key = Key;
    // End critical section
    ThreadSynchronization::getInstance()->UnlockIdKey();
    return key;
}

void Authentication::run() {
    while(1)  {
        syslog(LOG_DEBUG, "AUTHENTICATION: sending GET request");
        Http::HandleRequest(HttpConnection, HTTP_MSG_GET_IDKEY);
        
        usleep(1000000); 
    }
    return;
}
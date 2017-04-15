#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>

#include <json/json.h>
#include <json/json-forwards.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"
#include "ThreadSynchronization.hpp"
#include "Authentication.hpp"
#include "HttpThread.hpp"

int Authentication::Id=1234;
int Authentication::Key=6789;

Authentication::Authentication() {
    // parent class constructor is automatically called
}

int Authentication::HandleRequest() {
    RestClient::Response response;
    std::string url;

    url.append(BASE_API_PATH).append("idkey");
   
    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    HttpConnection->SetHeaders(headers);

    syslog(LOG_DEBUG, "AUTHENTICATION: sending GET %s", url.c_str());
    response = HttpConnection->get(url);
    return ParseResponse(response);
}

int Authentication::ParseResponse(RestClient::Response response) {
    
    Json::Value root;
    Json::Reader reader;
    int id;
    int key;

    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_DEBUG, "AUTHENTICATION: response on GET idkey= (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (id=root.get("id" , 0).asInt()==0) { // defaults to 0 if not found in body
            syslog(LOG_DEBUG, "AUTHENTICATION: id' not found in response");
            return 0;
        } 
        if (id=root.get("key" , 0).asInt()==0) { // defaults to 0 if not found in body
            syslog(LOG_DEBUG, "AUTHENTICATION: key not found in response");
            return 0;
        }       
        syslog(LOG_DEBUG, "AUTHENTICATION: receive Id:%i Key:%i", id, key);
    } else {
        syslog(LOG_DEBUG, "AUTHENTICATION: parsing failed");
        return 0;

    }
    syslog(LOG_DEBUG, "AUTHENTICATION: id and key updated");
    Update(id, key);

    return 1;
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
        if (HandleRequest()) {
            syslog(LOG_DEBUG, "AUTHENTICATION: finished, thread killed");
            stop();
        }
        usleep(1000000); 
    }
    return;
}
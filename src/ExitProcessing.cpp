#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include <json/json.h>
#include <json/json-forwards.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "ExitProcessing.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

ExitProcessing::ExitProcessing() {
    
    // compose url
    char id[10];
    sprintf(id, "%d", Authentication::GetId());
    //url.append(BASE_API_PATH).append(id).append("/parking").append("/entrance");
   
    url.append("/imber/barrier/false");

    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    headers["Token"] = Authentication::GetToken();
    HttpConnection->SetHeaders(headers);

    // compose body
    //char key[10];
    //sprintf(key, "%d", Authentication::GetKey());
    //body.append("{\"key\": ").append(key).append("\"}");
}

int ExitProcessing::HandleRequest() {
    RestClient::Response response;

    if (Settings::PLCWorksAutonomously()) {
        syslog(LOG_INFO, "EXIT: autonomous (allowed)");
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
        return 1;
    } else {
        syslog(LOG_DEBUG, "EXIT: sending GET %s", url.c_str());
        response = HttpConnection->get(url);
        return ParseResponse(response);
    }
}

int ExitProcessing::ParseResponse(RestClient::Response response) {
    Json::Value root;
    Json::Reader reader;
    bool allowed;
   
    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "EXIT: response on GET = (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (!root.isMember("allowed")) {
            syslog(LOG_DEBUG, "EXIT: parameter 'allowed' not found in response");
        } else {
            allowed=root.get("allowed", false).asBool();
        }
    } else {
        syslog(LOG_DEBUG, "EXIT: parsing failed");
        return 0;
    }

    if (allowed) {
        syslog(LOG_INFO, "EXIT: access allowed");
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
    } else {
        syslog(LOG_INFO, "EXIT: access NOT allowed");
    }

    return 1;
}

void ExitProcessing::run() {
    bool bOnLoop=false;
    bool bWasOnLoop=false;

    syslog(LOG_INFO, "EXIT: thread started");

    while(1)  {
        
        bOnLoop = IOHandler::GetIO("ExitLoopAct");
       
        if (bOnLoop && !bWasOnLoop) {
            usleep(3000000);
            bOnLoop = IOHandler::GetIO("ExitLoopAct");
            if (bOnLoop) {
                bWasOnLoop=true;
                syslog(LOG_INFO, "EXIT: on loop");
                HandleRequest();
            }
        } else if (!bOnLoop) {
            bWasOnLoop=false;
        }
    
        usleep(100000); 
    }
    return;
}
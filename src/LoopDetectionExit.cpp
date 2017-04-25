#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>
#include <sys/time.h>
#include <string.h>

#include <json/json.h>
#include <json/json-forwards.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "LoopDetectionEntrance.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

LoopDetectionExit::LoopDetectionExit() {
    
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
    headers["Key"] = Authentication::GetKey();
    HttpConnection->SetHeaders(headers);

    // compose body
    //char key[10];
    //sprintf(key, "%d", Authentication::GetKey());
    //body.append("{\"key\": ").append(key).append("\"}");
}

int LoopDetectionExit::HandleRequest() {
    RestClient::Response response;

    syslog(LOG_DEBUG, "LOOPDETECTION EXIT: sending GET %s", url.c_str());
    response = HttpConnection->get(url);
    return ParseResponse(response);
}

int LoopDetectionExit::ParseResponse(RestClient::Response response) {
    Json::Value root;
    Json::Reader reader;
    bool allowed;
   
    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "LOOPDETECTION EXIT: response on GET = (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (!root.isMember("allowed")) {
            syslog(LOG_DEBUG, "LOOPDETECTION EXIT: parameter 'allowed' not found in response");
        } else {
            allowed=root.get("allowed", false).asBool();
        }
    } else {
        syslog(LOG_DEBUG, "LOOPDETECTION EXIT: parsing failed");
        return 0;
    }

    if (allowed) {
        syslog(LOG_INFO, "LOOPDETECTION EXIT: access allowed");
#if SITE_ZWIJNAARDSESTEENWEG
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#else
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
#endif
    } else {
        syslog(LOG_INFO, "LOOPDETECTION EXIT: access NOT allowed");
    }

    return 1;
}

#if SITE_ZWIJNAARDSESTEENWEG

void LoopDetectionExit::run() {
    
    bool bOnEntranceLoop=false;
    bool bOnEntranceLoop_d=false;
    bool bEntranceDetected=false;
    bool bEntranceSequence=false;

    syslog(LOG_INFO, "LOOPDETECTION EXIT: thread started");

    while(1)  {
        
        bOnEntranceLoop = IOHandler::GetIO("ExitLoopAct");

        // exit handling
        // rule: a vehicle on the entrance loop will only trigger the LPR when 
        // when there is no vehicle on the exit loop and when the entrance loop is at least activated for 2 seconds
        // In case there is a vehicle on the exit loop, it will wait until the exit loop is deactivated
        if (bOnEntranceLoop && !bOnEntranceLoop_d) {
            bEntranceDetected=true;
        } 

        bOnEntranceLoop_d = bOnEntranceLoop;      
        usleep(100000); 
    }
    }
    return;
}
#else

void LoopDetectionExit::run() {
    bool bOnLoop=false;
    bool bWasOnLoop=false;

    syslog(LOG_INFO, "LOOPDETECTION EXIT: thread started");

    while(1)  {
        
        bOnLoop = IOHandler::GetIO("ExitLoopAct");
       
        if (bOnLoop && !bWasOnLoop) {
            usleep(3000000);
            bOnLoop = IOHandler::GetIO("ExitLoopAct");
            if (bOnLoop) {
                bWasOnLoop=true;
                syslog(LOG_INFO, "LOOPDETECTION EXIT: on loop");
                HandleRequest();
            }
        } else if (!bOnLoop) {
            bWasOnLoop=false;
        }
    
        usleep(100000); 
    }
    return;
}
    
#endif
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

#include "LoopDetectionEntrance.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

LoopDetectionEntrance::LoopDetectionEntrance() {
    
    // compose url
    char id[10];
    sprintf(id, "%d", Authentication::GetId());
    //url.append(BASE_API_PATH).append(id).append("/parking").append("/entrance");
   
    url.append("/imber/barrier/true");

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

int LoopDetectionEntrance::HandleRequest() {
    RestClient::Response response;

    if (Settings::PLCWorksAutonomously()) {
        syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: autonomous (allowed)");
#ifdef SITE_ZWIJNAARDSESTEENWEG
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#else
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#endif
        return 1;
    } else {
        syslog(LOG_DEBUG, "LOOPDETECTION ENTRANCE: sending GET %s", url.c_str());
        response = HttpConnection->get(url);
        return ParseResponse(response);
    }
}

int LoopDetectionEntrance::ParseResponse(RestClient::Response response) {
    Json::Value root;
    Json::Reader reader;
    bool allowed;
   
    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: response on GET = (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (!root.isMember("allowed")) {
            syslog(LOG_DEBUG, "LOOPDETECTION ENTRANCE: parameter 'allowed' not found in response");
        } else {
            allowed=root.get("allowed", false).asBool();
        }
    } else {
        syslog(LOG_DEBUG, "LOOPDETECTION ENTRANCE: parsing failed");
        return 0;
    }

    if (allowed) {
        syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: access allowed");
#ifdef SITE_ZWIJNAARDSESTEENWEG
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#else
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#endif
    } else {
        syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: access NOT allowed");
    }

    return 1;
}

#ifdef SITE_ZWIJNAARDSESTEENWEG

void LoopDetectionEntrance::run() {
    
    bool bOnEntranceLoop=false;
    bool bOnExitLoop=false;
    bool bOnEntranceLoop_d=false;
    bool bEntranceDetected=false;
    bool bEntranceSequence=false;
    long long EntranceSequenceStartTime;

    syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: thread started");

    while(1)  {
        
        bOnEntranceLoop = IOHandler::GetIO("EntranceLoopAct");
        bOnExitLoop     = IOHandler::GetIO("ExitLoopAct");

        // entrance handling
        // rule: a vehicle on the entrance loop will only trigger the LPR when 
        // when there is no vehicle on the exit loop and when the entrance loop is at least activated for 2 seconds
        // In case there is a vehicle on the exit loop, it will wait until the exit loop is deactivated
        if (bOnEntranceLoop && !bOnEntranceLoop_d) {
            bEntranceDetected=true;
            syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: detected");
        } 

        if (bEntranceDetected) {
            if (!bOnExitLoop) {
                bEntranceDetected = false;
                bEntranceSequence = true;
                EntranceSequenceStartTime = Settings::CurrentTimeInMilliseconds();
                syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: sequence started");
            } else {
                //wait until lus uitrit deactivated;
                syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: vehicle on exit loop");
            }
        }

        if (bEntranceSequence) {
            if (!bOnEntranceLoop) { // vehicle leaves entrance loop 
                bEntranceSequence=false;
                syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: sequence aborted");
            } else if ((Settings::CurrentTimeInMilliseconds()-EntranceSequenceStartTime) > 2000) {
                if (bOnEntranceLoop) {
                    bEntranceSequence = false;
                    //syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: open barrier");
                    HandleRequest();
                }
            } else {
                syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: wait (%lld)", Settings::CurrentTimeInMilliseconds());
            }
        }

        bOnEntranceLoop_d = bOnEntranceLoop;  
        //syslog(LOG_INFO, "%lld", CurrentTimeInMilliseconds());
        usleep(500000); 
    }
    
    return;
}
#else

void LoopDetectionEntrance::run() {
    bool bOnLoop=false;
    bool bWasOnLoop=false;

    syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: thread started");

    while(1)  {
        
        bOnLoop = IOHandler::GetIO("EntranceLoopAct");
       
        if (bOnLoop && !bWasOnLoop) {
            usleep(3000000);
            bOnLoop = IOHandler::GetIO("EntranceLoopAct");
            if (bOnLoop) {
                bWasOnLoop=true;
                syslog(LOG_INFO, "LOOPDETECTION ENTRANCE: on loop");
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
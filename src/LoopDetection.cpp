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

#include "LoopDetection.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

LoopDetection::LoopDetection(GateType type) {
    // parent class constructor is automatically called
    loop = type;

    switch (loop) {
        case ENTRANCE:
            sprintf(loopstring, "entrance"); break;
        case EXIT:
            sprintf(loopstring, "exit"); break;
    }

    // compose url
    char id[10];
    sprintf(id, "%d", Authentication::GetId());
    //if (loop==ENTRANCE) url.append(BASE_API_PATH).append(id).append("/parking").append("/entrance");
    //else                url.append(BASE_API_PATH).append(id).append("/parking").append("/exit");
   
    if (loop==ENTRANCE) url.append("/imber/barrier/true");
    else url.append("/imber/barrier/false");

    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    HttpConnection->SetHeaders(headers);

    // compose body
    char key[10];
    sprintf(key, "%d", Authentication::GetKey());
    body.append("{\"key\": ").append(key).append("\"}");

    // Configure timer
    //struct sigaction sa;
    /* Install timer handler for entrance loop detection */
    //memset (&sa, 0, sizeof (sa));
    //sa.sa_handler = &timerCallback;
    //sigaction (SIGVTALRM, &sa, NULL);
}

int LoopDetection::HandleRequest() {
    RestClient::Response response;

    syslog(LOG_DEBUG, "LOOPDETECTION: sending POST %s %s", loopstring, url.c_str());
    //response = HttpConnection->post(url, body);
    response = HttpConnection->get(url);
    return ParseResponse(response);
}

int LoopDetection::ParseResponse(RestClient::Response response) {
    Json::Value root;
    Json::Reader reader;
    bool allowed;
   
    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "LOOPDETECTION: response on POST %s = (%i) %s", loopstring, response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (!root.isMember("allowed")) {
            syslog(LOG_DEBUG, "LOOPDETECTION: parameter 'allowed' not found in response on %s", loopstring);
        } else {
            allowed=root.get("allowed", false).asBool();
        }
    } else {
        syslog(LOG_DEBUG, "LOOPDETECTION: parsing failed");
        return 0;
    }

    if (allowed) {
        syslog(LOG_INFO, "LOOPDETECTION: access allowed on %s", loopstring);
        ThreadSynchronization::ReleaseBarrierSemaphore();
    } else {
        syslog(LOG_INFO, "LOOPDETECTION: access NOT allowed on %s", loopstring);
    }

    return 1;
}

/*
void LoopDetection::fireTimer() {

    //Configure the timers to expire after .... msec. (and no repeat) 
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = Settings::GetLoopTimeout() * 1000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}*/

// Callback, called when timer of entry loop ends
// Time during which vehicle should be on entry loop
/*
void LoopDetection::timerCallback(int signum) {
    uint8_t bOnLoop;

    //bOnLoop = IOHandler::GetIO("EntranceLoopAct");
    if (bOnLoop) {
        if( !Settings::PLCWorksAutonomously()) {    
            syslog(LOG_DEBUG, "VEHICLE_DETECTION: sending POST request");
            HandleRequest();
        } else {
            // trigger barrier directly
          
        }
   }
}*/

bool LoopDetection::onLoop() {

    switch (loop) {
        case ENTRANCE:
            return IOHandler::GetIO("EntranceLoopAct");
        case EXIT:
            return IOHandler::GetIO("ExitLoopAct");
    }
}

void LoopDetection::run() {
    bool bOnLoop=false;
    bool bWasOnLoop=false;

    syslog(LOG_INFO, "LOOPDETECTION: thread started");

    while(1)  {
        // entry loop
        
        bOnLoop = onLoop();

        if (bOnLoop && !bWasOnLoop) {

            usleep(3000000);
            if (onLoop()) {
                bWasOnLoop=true;
                syslog(LOG_INFO, "LOOPDETECTION: on %s loop", loopstring);
                HandleRequest();
            }
        } else if (!bOnLoop) {
            bWasOnLoop=false;
        }
    
        usleep(100000); 
    }
    return;
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>
#include <sys/time.h>
#include <string.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "LoopDetection.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"

using namespace onposix;

LoopDetection::LoopDetection(GateType type) {
    // parent class constructor is automatically called
    loop = type;

    // compose url
    char id[10];
    sprintf(id, "%d", Authentication::GetId());
    if (loop==ENTRANCE) url.append(BASE_API_PATH).append(id).append("/parking").append("/entrance");
    else                url.append(BASE_API_PATH).append(id).append("/parking").append("/exit");
   
    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    HttpConnection->SetHeaders(headers);

    // compose body
    char key[10];
    sprintf(key, "%d", Authentication::GetKey());
    body.append("{\"key\": \"").append(key).append("\"}");

    // Configure timer
    struct sigaction sa;
    /* Install timer handler for entrance loop detection */
    memset (&sa, 0, sizeof (sa));
    //sa.sa_handler = &timerCallback;
    sigaction (SIGVTALRM, &sa, NULL);
}

int LoopDetection::HandleRequest() {
    RestClient::Response response;

    syslog(LOG_DEBUG, "LOOPDETECTION: sending POST %s", url.c_str());
    response = HttpConnection->post(url, body);
    return ParseResponse(response);
}

int LoopDetection::ParseResponse(RestClient::Response response) {
    return 0;
}

void LoopDetection::fireTimer() {

    /* Configure the timers to expire after .... msec. (and no repeat) */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = Settings::GetLoopTimeout();
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
}

// Callback, called when timer of entry loop ends
// Time during which vehicle should be on entry loop
void LoopDetection::timerCallback(int signum) {
    uint8_t bOnLoop;

    //bOnLoop = IOHandler::getInstance()->GetIO("EntranceLoopAct");
    if (bOnLoop) {
        if( !Settings::PLCWorksAutonomously()) {    
            syslog(LOG_DEBUG, "VEHICLE_DETECTION: sending POST request");
            HandleRequest();
        } else {
            // trigger barrier directly
          
        }
   }
}

void LoopDetection::run() {
    uint32_t bOnLoop=0;
    uint32_t bWasOnLoop=0;
    bool GotApiIdKey=true;
    
    while(1)  {
        bOnLoop = IOHandler::getInstance()->GetIO("EntranceLoopAct");

        // entry loop
        if (bOnLoop && !bWasOnLoop) { // rising edge detection
            bWasOnLoop = true;
            fireTimer();
        } 
        else if (!bOnLoop) { // reset when vehicle leaves loop
            bWasOnLoop = false;
        }
    
        usleep(100000); 
    }
    return;
}
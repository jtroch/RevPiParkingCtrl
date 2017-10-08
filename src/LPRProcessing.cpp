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

#include "LPRProcessing.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

LPRProcessing::LPRProcessing() {
    
    // compose url
    char id[10];
    sprintf(id, "%d", Authentication::GetId());
    syslog(LOG_INFO, "LPRProcessing: token=%s",Authentication::GetToken().c_str());
    
    // composing routes
    barrierUrl.append(BASE_API_PATH);
    barrierUrl.append("/barrier");
    loopUrl.append(BASE_API_PATH);
    loopUrl.append("/loop");

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

int LPRProcessing::HandleRequest() {
    return 0;
}

int LPRProcessing::HandleBarrierRequest() {
    RestClient::Response response;

    syslog(LOG_INFO, "LPRPROCESSING: sending barrier request GET %s", barrierUrl.c_str());
    response = HttpConnection->get(barrierUrl);
    return ParseBarrierResponse(response);
}

int LPRProcessing::ParseBarrierResponse(RestClient::Response response) {
    Json::Value root;
    Json::Reader reader;
    bool open;
   
    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_DEBUG, "LPRPROCESSING: response on GET = (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (!root.isMember("open")) {
            syslog(LOG_DEBUG, "LPRPROCESSING: parameter 'open' not found in response");
        } else {
            open=root.get("open", false).asBool();
        }
    } else {
        syslog(LOG_DEBUG, "LPRPROCESSING: parsing failed");
        return 0;
    }

    IOHandler::SetIO("OpenEntrance", open);

    return 1;
}

int LPRProcessing::HandleLoopRequest(int loop) {
    RestClient::Response response;
    Json::Value root;
    char body[30];

    root["loop"] = loop;
    
    sprintf(body, "{/"loop/":%d}", loop);
    syslog(LOG_INFO, "LPRPROCESSING: sending loop request POST %s with body %s", loopUrl.c_str(), body);

    response = HttpConnection->post(loopUrl, body);
    return ParseBarrierResponse(response);
}

void LPRProcessing::run() {
    
    bool bOnEntranceLoop=false;
    bool bOnExitLoop=false;
    bool bOnSafetyLoop=false;
    bool bOnEntranceLoop_d=false;
    bool bOnExitLoop_d=false;
    bool bOnSafetyLoop_d=false;

    syslog(LOG_INFO, "LPRPROCESSING: thread started");

    while(1)  {
        
        bOnEntranceLoop = IOHandler::GetIO("EntranceLoopAct");
        bOnExitLoop     = IOHandler::GetIO("ExitLoopAct");
        bOnSafetyLoop   = IOHandler::GetIO("SafetyLoopAct");

        HandleBarrierRequest();

        if ((bOnEntranceLoop_d==false) && (bOnEntranceLoop==true)) {
            HandleLoopRequest(1);
        }
        if ((bOnSafetyLoop_d==false) && (bOnSafetyLoop==true)) {
            HandleLoopRequest(2);
        }
        if ((bOnExitLoop_d==false) && (bOnExitLoop==true)) {
            HandleLoopRequest(3);
        }
        
        bOnEntranceLoop_d = bOnEntranceLoop;
        bOnSafetyLoop_d   = bOnSafetyLoop;
        bOnExitLoop_d     = bOnExitLoop;

        usleep(5000000); 
    }
    
    return;
}
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

#include "CentralProcessing.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

CentralProcessing::CentralProcessing() {
    
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

int CentralProcessing::HandleRequest() {
    RestClient::Response response;

    if (Settings::PLCWorksAutonomously()) {
        syslog(LOG_INFO, "CENTRALPROCESSING: request (autonomous=allowed)");

#ifdef SITE_ZWIJNAARDSESTEENWEG
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#else
        if (type==ENTRANCE)
            ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
        else 
            ThreadSynchronization::ReleaseExitBarrierSemaphore();
#endif
        return 1;
    } else {
        syslog(LOG_DEBUG, "CENTRALPROCESSING: sending GET %s", url.c_str());
        response = HttpConnection->get(url);
        return ParseResponse(response);
    }
}

int CentralProcessing::ParseResponse(RestClient::Response response) {
    Json::Value root;
    Json::Reader reader;
    bool allowed;
   
    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "CENTRALPROCESSING: response on GET = (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if (!root.isMember("allowed")) {
            syslog(LOG_DEBUG, "CENTRALPROCESSING: parameter 'allowed' not found in response");
        } else {
            allowed=root.get("allowed", false).asBool();
        }
    } else {
        syslog(LOG_DEBUG, "CENTRALPROCESSING: parsing failed");
        return 0;
    }

    if (allowed) {
        syslog(LOG_INFO, "CENTRALPROCESSING: access allowed");
#ifdef SITE_ZWIJNAARDSESTEENWEG
        ThreadSynchronization::ReleaseExitBarrierSemaphore();
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#else
        ThreadSynchronization::ReleaseEntranceBarrierSemaphore();
#endif
    } else {
        syslog(LOG_INFO, "CENTRALPROCESSINGE: access NOT allowed");
    }

    return 1;
}

void CentralProcessing::run() {
    
    bool bOnEntranceLoop=false;
    bool bOnExitLoop=false;
    bool bOnEntranceLoop_d=false;
    bool bOnExitLoop_d=false;
    bool bTicket=false;
    bool bTicket_d=false;
    bool bMoney=false;
    bool bMoney_d=false;
    bool bTicketInserted=false;
    bool bMoneyInserted=false;

    bool bEntranceDetected=false;
    bool bExitDetected=false;
    bool bEntranceSequence1=false;
    bool bEntranceSequence2=false;
    bool bExitSequence=false;
    bool bHoldExitBarrier=false;
    bool bVehicleLeftExitLoop=false;
    long long EntranceSequenceStartTime;

    syslog(LOG_INFO, "CENTRALPROCESSING: thread started");

    while(1)  {
        
        bOnEntranceLoop = IOHandler::GetIO("EntranceLoopAct");
        bOnExitLoop     = IOHandler::GetIO("ExitLoopAct");
        bTicket         = IOHandler::GetIO("TicketAct");
        bMoney          = IOHandler::GetIO("MoneyAct");

        if (bOnEntranceLoop && !bOnEntranceLoop_d) {
            bEntranceDetected=true;
            syslog(LOG_INFO, "CENTRALPROCESSING: entrance detected");
        } 

        if (bOnExitLoop && !bOnExitLoop_d) {
            bExitDetected=true;
            syslog(LOG_INFO, "CENTRALPROCESSING: exit detected");
        } 

        if (bTicket && !bTicket_d) {
            bTicketInserted=true;
            syslog(LOG_INFO, "CENTRALPROCESSING: ticket inserted");
        } 
        if (bMoney && !bMoney_d) {
            bMoneyInserted=true;
            syslog(LOG_INFO, "CENTRALPROCESSING: money inserted");
        } 

        // entrance handling
        // rule: a vehicle on the entrance loop will only trigger the LPR when 
        // when there is no vehicle on the exit loop and when the entrance loop is at least activated for 2 seconds
        // In case there is a vehicle on the exit loop, it will wait until the exit loop is deactivated
        
        if (bEntranceSequence2) {
            if (!bOnExitLoop) {
                bEntranceSequence2 = false;
                bVehicleLeftExitLoop = true;
                syslog(LOG_INFO, "CENTRALPROCESSING: entrance conflict resolved");
            }
        }
        
        if (bEntranceDetected) {
            if (bOnExitLoop) {
                bEntranceSequence2 = true;
                syslog(LOG_INFO, "CENTRALPROCESSING: entrance conflict");
            } else if (!bOnExitLoop || bVehicleLeftExitLoop) {
                bEntranceDetected = false;
                bVehicleLeftExitLoop = false;
                bEntranceSequence1 = true;
                EntranceSequenceStartTime = Settings::CurrentTimeInMilliseconds();
                syslog(LOG_INFO, "CENTRALPROCESSING: normal sequence started (%s)", (bVehicleLeftExitLoop ? "vehicle left exit loop" : "!bOnExitLoop"));
            }
        }

        if (bEntranceSequence1) {
            if (!bOnEntranceLoop) { // vehicle leaves entrance loop 
                bEntranceSequence1=false;
                syslog(LOG_INFO, "CENTRALPROCESSING: sequence aborted");
            } else if ((Settings::CurrentTimeInMilliseconds()-EntranceSequenceStartTime) > 2000) {
                if (bOnEntranceLoop) {
                    bEntranceSequence1 = false;
                    syslog(LOG_INFO, "CENTRALPROCESSING: entrance request");
                    HandleRequest();
                }
            } else {
                syslog(LOG_INFO, "CENTRALPROCESSING: wait");
            }
        }


        // exit handling
        // rule: If a vehicle is on the exit loop and a ticket is inserted and there is no vehicle on the entrance loop, then the barrier will open
        // If there is a vehicle on the entrance loop while the output is activated, the the barrier will not open until the vehicle leaves the entrance

        if (bHoldExitBarrier && bExitSequence) {
            if (!bOnEntranceLoop) {
                bHoldExitBarrier=false;
                syslog(LOG_INFO, "CENTRALPROCESSING: exit request");
                HandleRequest();
            } else {
                syslog(LOG_INFO, "CENTRALPROCESSING: exit conflict");
            }
        }

        if (bExitSequence) { 
            if (!bOnExitLoop) {
                bExitSequence=false;
                syslog(LOG_INFO, "CENTRALPROCESSING: exit sequence finished");
            }
            if (bTicketInserted || bMoneyInserted) {
                bTicketInserted=false;
                bMoneyInserted=false;
                bHoldExitBarrier=true;
                syslog(LOG_INFO, "CENTRALPROCESSING: hold exit barrier");
            }
        }

        if (bExitDetected) {
            bExitSequence=true;
            bExitDetected=false;
            syslog(LOG_INFO, "CENTRALPROCESSING: vehicle sequence started");
        }

         
        // continue close of barrier in case of conflict
        // whenever both loops entrance and exit are active, the barrier should stay closed
        // We do this by activating the "continous closed input" of barrier
        if (bOnExitLoop && bOnEntranceLoop) {
            IOHandler::SetIO("ContinuClosed", true);
            syslog(LOG_INFO, "CENTRALPROCESSING: continu closed");
        } else {
            IOHandler::SetIO("ContinuClosed", false);
        }

        bOnEntranceLoop_d = bOnEntranceLoop;  
        bOnExitLoop_d = bOnExitLoop;  
        bTicket_d = bTicket;
        bMoney_d = bMoney;

        usleep(500000); 
    }
    
    return;
}
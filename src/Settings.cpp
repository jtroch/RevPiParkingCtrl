#include <syslog.h>

#include <json/json.h>
#include <json/json-forwards.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiParkingCtrl.hpp"
#include "ThreadSynchronization.hpp"
#include "Authentication.hpp"
#include "IOHandler.hpp"
#include "Settings.hpp"

bool Settings::EntranceCO=false;
bool Settings::ExitCO=false;
bool Settings::PLCAuto=false;
int  Settings::LoopTimeout=3000;
int  Settings::HttpTimeout=2000;
int  Settings::BarrierPulseLength=1000;
bool Settings::TestOutput=false;

Settings::Settings() {
    // parent class constructor is automatically called
    char id[10];

    sprintf(id, "%d", Authentication::GetId());
    url.append(BASE_API_PATH).append(id).append("/parking").append("/settings");
   
    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    HttpConnection->SetHeaders(headers);
}

int Settings::HandleRequest() {
    RestClient::Response response;

    syslog(LOG_INFO, "SETTINGS: sending GET %s", url.c_str());
    response = HttpConnection->get(url);
    return ParseResponse(response);
}

int Settings::ParseResponse(RestClient::Response response) {
    
    Json::Value root;
    Json::Reader reader;
    int id;
    int key;
    int loopTimeout;
    int httpTimeout;
    int pulseLength;

    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "SETTINGS: response on GET settings= (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        if ((key=root.get("key" , 0).asInt())==0) { // defaults to 0 if not found in body
            syslog(LOG_DEBUG, "SETTINGS: key not found in response");
            return 0;
        }  
        if (key==Authentication::GetKey()) {
            // parse all other parameters if key matches
            // Critical section
            ThreadSynchronization::SettingsMutex.lock();

            if (!root.isMember("entrance-open")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'entrance-open' not found in response");
            } else {
                EntranceCO=root.get("entrance-open", false).asBool();
            }
            if (!root.isMember("exit-open")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'exit-open' not found in response");
            } else {
                ExitCO=root.get("exit-open", false).asBool();
            }
            if (!root.isMember("autonomous-mode")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'autonomous-mode' not found in response");
            } else {
                PLCAuto=root.get("autonomous-mode", false).asBool();
            }
            if (!root.isMember("loop-timeout")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'loop-timeout' not found in response");
            } else {
                loopTimeout=root.get("loop-timeout", 1000).asInt();
                if ((loopTimeout>500) && (loopTimeout<5000)) LoopTimeout=loopTimeout;
                else syslog(LOG_DEBUG, "SETTINGS: not allowed value for parameter 'loop-timeout'");
            }
            if (!root.isMember("http-timeout")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'http-timeout' not found in response");
            } else {
                httpTimeout=root.get("http-timeout", 1000).asInt();
                if ((httpTimeout>500) && (httpTimeout<5000)) HttpTimeout=httpTimeout;
                else syslog(LOG_DEBUG, "SETTINGS: not allowed value for parameter 'http-timeout'");
            }
            if (!root.isMember("barrier-pulse-length")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'barrier-pulse-length' not found in response");
            } else {
                pulseLength=root.get("barrier-pulse-length", 1000).asInt();
                if ((pulseLength>500) && (pulseLength<5000)) BarrierPulseLength=pulseLength;
                else syslog(LOG_DEBUG, "SETTINGS: not allowed value for parameter 'barrier-pulse-Length'");
            }
            if (!root.isMember("test-output")) {
                syslog(LOG_DEBUG, "SETTINGS: parameter 'test-output' not found in response");
            } else {
                TestOutput=root.get("test-output", false).asBool();
                IOHandler::SetIO("TestOutput", TestOutput);
            }
            
            // End critical section
            ThreadSynchronization::SettingsMutex.unlock();
            syslog(LOG_INFO, "SETTINGS: settings updated");
            return 1;

        } else syslog(LOG_DEBUG, "SETTINGS: authentication failed");

    } else syslog(LOG_ERR, "SETTINGS: parsing failed");

    return 0;
}

int Settings::GetLoopTimeout() {
    // Critical section
    ThreadSynchronization::SettingsMutex.lock();
    return LoopTimeout;
    ThreadSynchronization::SettingsMutex.unlock();
}

int Settings::GetHttpTimeout() {
    // Critical section
    ThreadSynchronization::SettingsMutex.lock();
    return HttpTimeout;
    ThreadSynchronization::SettingsMutex.unlock();
}

int Settings::GetBarrierPulseLength() {
    // Critical section
    ThreadSynchronization::SettingsMutex.lock();
    return BarrierPulseLength;
    ThreadSynchronization::SettingsMutex.unlock();
}

bool Settings::PLCWorksAutonomously() {
    int plcauto_hw=0;
    bool ret=false;

    if ((plcauto_hw=IOHandler::GetIO("PLCAuto"))==1)
    syslog(LOG_DEBUG, "SETTINGS: HW PLCAuto = %i", plcauto_hw);

    ThreadSynchronization::SettingsMutex.lock();
    if ( (plcauto_hw==1) || PLCAuto) {
        syslog(LOG_DEBUG, "SETTINGS: PLC works autonomously", plcauto_hw);
        ret=true;
    } else {
        syslog(LOG_DEBUG, "SETTINGS: PLC communicates with LPR", plcauto_hw);
    }
    ThreadSynchronization::SettingsMutex.unlock();
    
    return ret;
}

bool Settings::BarrierContinuouslyOpen(GateType type) {
    int CO_hw=0;
    bool ret=false;

    ThreadSynchronization::SettingsMutex.lock();
    switch (type) {
        case ENTRANCE:
            CO_hw = IOHandler::GetIO("EntranceCO");  
            if ( (CO_hw==1) || EntranceCO) {
                syslog(LOG_DEBUG, "SETTINGS: Entrance continuously set open");
                ret=true;
            }
        case EXIT:
            CO_hw = IOHandler::GetIO("ExitCO");  
            if ( (CO_hw==1) || ExitCO) { 
                syslog(LOG_DEBUG, "SETTINGS: Exit continuously set open");
                ret=true;
            }
    }
    ThreadSynchronization::SettingsMutex.unlock();
    return ret;
}

void Settings::run() {
    
    syslog(LOG_INFO, "SETTINGS: thread started");

    while(1)  {
        //syslog(LOG_INFO, "SETTINGS: sending GET request");
        HandleRequest();
        
        usleep(15000000); 
    }
    return;
}
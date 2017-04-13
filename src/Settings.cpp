#include <syslog.h>
#include "RevPiParkingCtrl.hpp"
#include "ThreadSynchronization.hpp"
#include "IOHandler.hpp"
#include "Settings.hpp"
#include "Http.hpp"

bool Settings::EntranceBarrierContinuouslyOpen=false;
bool Settings::ExitBarrierContinuouslyOpen=false;
bool Settings::PLCWorksAuto=false;
int  Settings::DetectionLoopTimeout=3000;
int  Settings::HttpReplyTimeout=2000;
int  Settings::BarrierPulseLength=1000;
int  Settings::TestOutput=0;


Settings::Settings() {
    // get a connection object
    HttpConnection = new RestClient::Connection(SERVER_ADDRESS);
}

void Settings::Update(
    bool EntranceBarrierContinuouslyOpen,
    bool ExitBarrierContinuouslyOpen,
    bool PLCWorksAuto,
    int DetectionLoopTimeout,
    int HttpReplyTimeout,
    int BarrierPulseLength,
    int TestOutput
) {
    // Critical section
    ThreadSynchronization::getInstance()->LockSettings();
    
    EntranceBarrierContinuouslyOpen=EntranceBarrierContinuouslyOpen;
    ExitBarrierContinuouslyOpen=ExitBarrierContinuouslyOpen;
    PLCWorksAuto=PLCWorksAutonomously;
    DetectionLoopTimeout=DetectionLoopTimeout;
    HttpReplyTimeout=HttpReplyTimeout;
    BarrierPulseLength=BarrierPulseLength;
    TestOutput=TestOutput;

    // End critical section
    ThreadSynchronization::getInstance()->UnlockSettings();
}

bool Settings::PLCWorksAutonomously() {
    int externalHWsetting=0;
    bool ret=false;

    ThreadSynchronization::getInstance()->LockSettings();
    externalHWsetting = IOHandler::getInstance()->GetIO("PLCWorksAutonomously");
    if ( (externalHWsetting==1) || PLCWorksAuto) ret=true;
    ThreadSynchronization::getInstance()->UnlockSettings();

    return ret;
}

bool Settings::BarrierContinuouslyOpen(BarrierType type) {
    int externalHWsetting=0;
    bool ret=false;

    ThreadSynchronization::getInstance()->LockSettings();
    switch (type) {
        case ENTRANCE_BARRIER:
            externalHWsetting = IOHandler::getInstance()->GetIO("EntranceBarrierContinuouslyOpen");  
            if ( (externalHWsetting==1) || EntranceBarrierContinuouslyOpen) ret=true;
        case EXIT_BARRIER:
            externalHWsetting = IOHandler::getInstance()->GetIO("ExitBarrierContinuouslyOpen");  
            if ( (externalHWsetting==1) || ExitBarrierContinuouslyOpen) ret=true;
    }
    ThreadSynchronization::getInstance()->UnlockSettings();
    return ret;
}

int Settings::GetDetectionLoopTimeout() {
    // Critical section
    ThreadSynchronization::getInstance()->LockSettings();
    return DetectionLoopTimeout;
    ThreadSynchronization::getInstance()->UnlockSettings();
}

int Settings::GetHttpReplyTimeout() {
    // Critical section
    ThreadSynchronization::getInstance()->LockSettings();
    return HttpReplyTimeout;
    ThreadSynchronization::getInstance()->UnlockSettings();
}

int Settings::GetBarrierPulseLength() {
    // Critical section
    ThreadSynchronization::getInstance()->LockSettings();
    return BarrierPulseLength;
    ThreadSynchronization::getInstance()->UnlockSettings();
}

void Settings::run() {
    
    while(1)  {
        syslog(LOG_DEBUG, "SETTINGS: sending GET request");
        Http::HandleRequest(HttpConnection, HTTP_MSG_GET_SETTINGS);
        
        usleep(5000000); 
    }
    return;
}
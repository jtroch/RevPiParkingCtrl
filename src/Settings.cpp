#include <syslog.h>
#include "RevPiParkingCtrl.hpp"
#include "ThreadSynchronization.hpp"
#include "IOHandler.hpp"
#include "Settings.hpp"

bool Settings::EntranceBarrierContinuouslyOpen=false;
bool Settings::ExitBarrierContinuouslyOpen=false;
bool Settings::PLCWorksAuto=false;
int  Settings::DetectionLoopTimeout=3000;
int  Settings::HttpReplyTimeout=2000;
int  Settings::BarrierPulseLength=1000;
int  Settings::TestOutput=0;


Settings::Settings() {
    // parent class constructor is automatically called
}

int Settings::HandleRequest() {
    return 0;
}

int Settings::ParseResponse(RestClient::Response response) {
    return 0;
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

bool Settings::BarrierContinuouslyOpen(GateType type) {
    int externalHWsetting=0;
    bool ret=false;

    ThreadSynchronization::getInstance()->LockSettings();
    switch (type) {
        case ENTRANCE:
            externalHWsetting = IOHandler::getInstance()->GetIO("EntranceBarrierContinuouslyOpen");  
            if ( (externalHWsetting==1) || EntranceBarrierContinuouslyOpen) ret=true;
        case EXIT:
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
        HandleRequest();
        
        usleep(5000000); 
    }
    return;
}
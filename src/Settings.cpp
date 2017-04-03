#include "PevPiParkingCtrl.h"
#include "Settings.hpp"
#include <Logger.hpp>

bool Settings::EntranceBarrierContinuouslyOpen=false;
bool Settings::ExitBarrierContinuouslyOpen=false;
bool Settings::PLCWorksAutonomously=false;
int  Settings::DetectionLoopTimeout=3000;
int  Settings::HttpReplyTimeout=2000;
int  Settings::BarrierPulseLength=1000;
int  Settings::TestOutput=0;

static void Settings::Update(
    bool EntranceBarrierContinuouslyOpen,
    bool ExitBarrierContinuouslyOpen,
    bool PLCWorksAutonomously,
    int DetectionLoopTimeout,
    int HttpReplyTimeout,
    int BarrierPulseLength,
    int TestOutput
) {
    // Critical section
    ThreadSynchronization.LockSettings();
    
    this.EntranceBarrierContinuouslyOpen=EntranceBarrierContinuouslyOpen;
    this.ExitBarrierContinuouslyOpen=ExitBarrierContinuouslyOpen;
    this.PLCWorksAutonomously=PLCWorksAutonomously;
    this.DetectionLoopTimeout=DetectionLoopTimeout;
    this.HttpReplyTimeout=HttpReplyTimeout;
    this.BarrierPulseLength=BarrierPulseLength;
    this.TestOutput=TestOutput;

    // End critical section
    ThreadSynchronization.UnlockSettings();
}

static bool Settings::PLCWorksAutonomously() {
    IOHandler IOhandler = new IOHandler();
    uint32_t externalHWsetting;

    externalHWsetting = IOhandler.GetIO("PLCWorksAutonomously");
    
    if ( (externalHWsetting==1) || Settings.PLCWorksAutonomously) return true;
    else return false;
}

static bool Settings::BarrierContinuouslyOpen(BarrierType type) {
    IOHandler IOhandler = new IOHandler();
    uint32_t externalHWsetting;

    switch (type) {
        case ENTRANCE_BARRIER:
            externalHWsetting = IOhandler.GetIO("EntranceBarrierContinuouslyOpen");  
            if ( (externalHWsetting==1) || Settings.EntranceBarrierContinuouslyOpen) return true;
            else return false;
        case EXIT_BARRIER:
            externalHWsetting = IOhandler.GetIO("ExitBarrierContinuouslyOpen");  
            if ( (externalHWsetting==1) || Settings.ExitBarrierContinuouslyOpen) return true;
            else return false;
    }
}

static void Settings::GetDetectionLoopTimeout() {
    // Critical section
    ThreadSynchronization.LockSettings();
    return this.DetectionLoopTimeout;
    ThreadSynchronization.UnlockSettings();
}

static void Settings::GetHttpReplyTimeout() {
    // Critical section
    ThreadSynchronization.LockSettings();
    return this.HttpReplyTimeoutimeout;
    ThreadSynchronization.UnlockSettings();
}

static void Settings::GetBarrierPulseLength() {
    // Critical section
    ThreadSynchronization.LockSettings();
    return this.BarrierPulseLength;
    ThreadSynchronization.UnlockSettings();
}

void Settings::run() {
    
    while(1)  {
        this.HttpWorkQueue.push(HTTP_GET_SETTINGS);
        usleep(SETTINGS_UPDATE_INTERVAL); 
    }
    return NULL;
}
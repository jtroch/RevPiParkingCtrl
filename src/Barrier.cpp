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

#include "Barrier.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Settings.hpp"

using namespace onposix;

Barrier::Barrier(GateType type) {
    
    barrier = type;

    // Configure timers
    struct sigaction sa;
    
    /* Install timer handler for entrance barrier */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &TimerCallback;
    sigaction (SIGVTALRM, &sa, NULL);
}

int Barrier::HandleRequest() {
    return 0;
}

int Barrier::ParseResponse(RestClient::Response response) {
    return 0;
}

void Barrier::SetBarrier(int value)  {
    switch (type) {
        case ENTRANCE:
            IOHandler::getInstance->SetIO("EntranceBarrier", value);
            break;
        case EXIT:
            IOHandler::getInstance->SteIO("ExitBarrier", value);
            break;
    }
}

void Barrier::FireTimer() {

    /* Configure the timers to expire after .... msec. (and no repeat) */
    Timer.it_value.tv_sec = 0;
    Timer.it_value.tv_usec = Settings::GetBarrierPulseLength();
    Timer.it_interval.tv_sec = 0;
    Timer.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &Timer, NULL);
}

// Callback, called when timer ends
void Barrier::TimerCallback(int signum) {
   
    if (!Settings::BarrierContinuouslyOpen(barrier)) // do not close if set to be continuously open bij HW pin
    {
        // close barrier (= actualy don't drive open anymore')
        SetBarrier(0);
    }
}

void Barriers::run() {
    
    
    while(1)  {
        
        // wait for trigger by Http thread
        if (...) {
            // open barrier 
            SetBarrier(1);
            FireTimer();
        }

        if (Settings::BarrierContinuouslyOpen(barrier)) {
            SetBarrier(1);
        }

        usleep(SETTINGS_UPDATE_INTERVAL); 
    }
    return NULL;
}
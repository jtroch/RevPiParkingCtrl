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
#include "ThreadSynchronization.hpp"

using namespace onposix;

Barrier::Barrier(GateType type) {
    barrier = type;
}

void Barrier::setBarrier(bool open)  {
    switch (barrier) {
        case ENTRANCE:
            IOHandler::SetIO("OpenEntrance", open);
            break;
        case EXIT:
            IOHandler::SetIO("OpenExit", open);
            break;
        case ENTRANCE_EXIT:
            IOHandler::SetIO("OpenEntrance", open);
            break;
    }
}

void Barrier::run() {
    
    syslog(LOG_INFO, "BARRIER: thread started");

    while(1)  {
        
        ThreadSynchronization::AcquireBarrierSemaphore();
        setBarrier(true);
        syslog(LOG_INFO, "BARRIER: opened");
        usleep(1000000);
    
        if (Settings::BarrierContinuouslyOpen(barrier)) {
            setBarrier(true);
        } else {
            setBarrier(false);
        }

        usleep(100000); 
    }
    return;
}
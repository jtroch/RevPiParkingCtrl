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
    if (barrier==ENTRANCE) {
        IOHandler::SetIO("OpenEntrance", open);
    } else {
        IOHandler::SetIO("OpenExit", open);
    }
}

void Barrier::run() {
    
    syslog(LOG_INFO, "BARRIER: thread started");

    while(1)  {
        if (barrier==ENTRANCE) {
            ThreadSynchronization::AcquireEntranceBarrierSemaphore();
            syslog(LOG_INFO, "BARRIER: entrance opening");
        } else {
            ThreadSynchronization::AcquireExitBarrierSemaphore();
            syslog(LOG_INFO, "BARRIER: exit opening");
        }
       
        setBarrier(true);
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